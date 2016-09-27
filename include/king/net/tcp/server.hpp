//一個 asio tcp 服務器
#ifndef KING_LIB_HEADER_NET_TCP_SERVER
#define KING_LIB_HEADER_NET_TCP_SERVER

#include "type.hpp"



#include <boost/function.hpp>
#include <boost/bind.hpp>


namespace king
{
namespace net
{
namespace tcp
{
    template<typename T>
    class server_t
    {
    protected:
        typedef king::net::tcp::socket_t<T> socket_t;
        typedef std::shared_ptr<socket_t> socket_spt;

        //io 服務
        io_service_t _io_s;

        //recv 緩衝區 大小
        std::size_t _buffer;


        //連接 接受器
        acceptor_t _acceptor;

        //工作 線程
        boost::thread_group _threads;
        void work_thread()
        {
            _io_s.run();
        }

    public:
        explicit server_t(const unsigned short port,const std::size_t buffer = 1024 * 8) //throw boost::system::system_error
            :_acceptor(_io_s,boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(),port)),
            _buffer(buffer)
        {
            //異步 接受 連接
            std::size_t count = (boost::thread::hardware_concurrency() + 1 ) * 2;
            for(std::size_t i = 0 ; i < count ; ++i)
            {
               post_accept();
            }

        }
        server_t& operator=(const server_t&) = delete;
        server_t(const server_t&) = delete;
        ~server_t()
        {
            _io_s.stop();
            _threads.join_all();
        }

    protected:
        //連接 回調
        typedef boost::function<void(server_t*,socket_spt)> accept_bft;
        accept_bft _accept_bf;

        typedef boost::function<void(server_t*,socket_spt)> close_bft;
        close_bft _close_bf;

        typedef boost::function<void(server_t*,socket_spt,const king::byte_t*,std::size_t n)> recv_bft;
        recv_bft _recv_bf;

        typedef boost::function<void(server_t*,socket_spt,bytes_spt)> send_bft;
        send_bft _send_bf;
    public:
        inline void accept_bf(accept_bft bf)
        {
            _accept_bf = bf;
        }
        inline accept_bft accept_bf()const
        {
            return _accept_bf;
        }

        inline void close_bf(close_bft bf)
        {
            _close_bf = bf;
        }
        inline close_bft close_bf()const
        {
            return _close_bf;
        }

        inline void recv_bf(recv_bft bf)
        {
            _recv_bf = bf;
        }
        inline recv_bft recv_bf()const
        {
            return _recv_bf;
        }

        inline void send_bf(send_bft bf)
        {
            _send_bf = bf;
        }
        inline send_bft send_bf()const
        {
            return _send_bf;
        }


    protected:
        inline void post_accept()
        {
            socket_spt s = std::make_shared<socket_t>(_io_s);
            _acceptor.async_accept(s->socket(),
                boost::bind(&server_t::post_accept_handler,
                this,
                boost::asio::placeholders::error,
                s)
            );
        }
        void post_accept_handler(const boost::system::error_code& e,socket_spt s)
        {
            //投遞 新的 接受 操作
            post_accept();

            //連接錯誤 直接返回
            if(e)
            {
                return;
            }

            //創建 recv 緩衝區
            bytes_spt buffer;
            try
            {
                buffer = std::make_shared<king::bytes::bytes_t>(_buffer);
                if(buffer->empty())
                {
                    //創建 recv 緩衝區失敗 直接 斷開連接
                    return;
                }
            }
            catch(const std::bad_alloc&)
            {
                //創建 recv 緩衝區失敗 直接 斷開連接
                return;
            }


            //通知 用戶
            if(_accept_bf)
            {
                _accept_bf(this,s);
            }
            //投遞 異步 recv
            post_recv(s,buffer);

        }
        inline void post_recv(socket_spt s,bytes_spt buffer)
        {
            s->socket().async_read_some(boost::asio::buffer(buffer->get(),buffer->size()),
                boost::bind(&server_t::post_recv_handler,
                this,
                boost::asio::placeholders::error,
                s,
                buffer,
                boost::asio::placeholders::bytes_transferred)
            );
        }
        void post_recv_handler(const boost::system::error_code& e,socket_spt s,bytes_spt buffer,std::size_t n)
        {
            if(e)
            {
                //通知 用戶
                if(_close_bf)
                {
                    _close_bf(this,s);
                }

                //錯誤 斷開 連接
                if(s->socket().is_open())
                {
                    boost::system::error_code e0;
                    s->socket().close(e0);
                }

                return;
            }
            //通知 用戶
            if(_recv_bf)
            {
                _recv_bf(this,s,buffer->get(),n);
            }

            //投遞 新的 recv
            post_recv(s,buffer);
        }
    public:
        //啟動 工作 線程
        void run()
        {
            if(_threads.size())
            {
                return;
            }

            std::size_t count = (boost::thread::hardware_concurrency() + 1 ) * 2;
            for(std::size_t i = 0 ; i < count ; ++i)
            {
                _threads.add_thread(new boost::thread(boost::bind(&server_t::work_thread,this)));
            }

        }
        //停止 服務
        inline void stop()
        {
            _io_s.stop();
        }
        //返回 服務是否 停止
        inline bool stopped()const
        {
            return _io_s.stopped();
        }
        //返回 工作 線程 數量
        inline std::size_t work_threads()const
        {
            return _threads.size();
        }

        //等待 線程 停止 工作
        inline void join()
        {
            _threads.join_all();
        }

        //向 客戶端 發送 隊列 寫入一條 發送 數據
        bool push_back_write(socket_spt s,const byte_t* bytes,std::size_t n)
        {
            if(!s->socket().is_open())
            {
                return false;
            }

            //創建 write 緩衝區
            bytes_spt buffer;
            try
            {
                buffer = std::make_shared<king::bytes::bytes_t>(n);
                if(buffer->empty())
                {
                    //創建 失敗
                    return false;
                }
            }
            catch(const std::bad_alloc&)
            {
                //創建 失敗
                return false;
            }
            //copy 待write 數據
            std::copy(bytes,bytes+n,buffer->get());

            return push_back_write(s,buffer);
        }
        bool push_back_write(socket_spt s,bytes_spt buffer)
        {
            if(!s->socket().is_open())
            {
                return false;
            }

            boost::mutex::scoped_lock lock(s->_mutex);
            auto& datas = s->_datas;
            auto& wait = s->wait;

            //等待 上次 write 完成 直接 push
            if(wait)
            {
                datas.push_back(buffer);
                return true;
            }
            else
            {
                //需要 發送 數據

                if(datas.empty())
                {
                    //直接 write
                    post_write(s,buffer);
                    wait = true;
                    return true;
                }
                else
                {
                    try
                    {
                        //寫入 隊列
                        datas.push_back(buffer);

                        //發送 隊列 首數據
                        buffer = datas.front();
                        datas.pop_front();
                        post_write(s,buffer);
                        wait = true;
                        return true;
                    }
                    catch(const std::bad_alloc&)
                    {
                        return false;
                    }

                }

            }
            return false;
        }
    protected:
        inline void post_write(socket_spt s,bytes_spt buffer)
        {
            s->socket().async_write_some(boost::asio::buffer(buffer->get(),buffer->size()),
                boost::bind(&server_t::post_write_handler,
                this,
                boost::asio::placeholders::error,
                s,
                buffer
                )
            );
        }
        void post_write_handler(const boost::system::error_code& e,socket_spt s,bytes_spt buffer)
        {
            if(e)
            {
                //send 錯誤 直接 關閉
                if(s->socket().is_open())
                {
                    boost::system::error_code e0;
                    s->socket().close(e0);
                }
                return;
            }

            //通知 客戶
            if(_send_bf)
            {
                _send_bf(this,s,buffer);
            }


            boost::mutex::scoped_lock lock(s->_mutex);
            auto& datas = s->_datas;
            if(datas.empty())
            {
                //接受 數據 發送
                s->wait = false;
                return;
            }
            //繼續 發送 數據
            buffer = datas.front();
            datas.pop_front();
            post_write(s,buffer);

        }


    };


};
};
};

#endif // KING_LIB_HEADER_NET_TCP_SERVER
