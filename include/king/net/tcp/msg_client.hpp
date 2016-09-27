//一個 帶消息邊界的 asio tcp 服務器
//依賴 server_t
#ifndef KING_LIB_HEADER_NET_TCP_MSG_CLIENT
#define KING_LIB_HEADER_NET_TCP_MSG_CLIENT

#include "type.hpp"


#include "client.hpp"
#include "msg_reader.hpp"


namespace king
{
namespace net
{
namespace tcp
{

    template<typename T,std::size_t N=1024*8>
    class msg_client_t
    {
    protected:
        typedef msg_data<T,N> user_t;
        typedef king::net::tcp::socket_t<user_t> socket_t;
    public:
        typedef std::shared_ptr<socket_t> socket_spt;
    protected:
        typedef client_t<user_t> client_t;

        client_t _s;
        msg_reader_t _msg_reader;

    public:
        explicit msg_client_t(const std::string& addr,const unsigned short port,const msg_reader_t& reader,const std::size_t buffer = 1024 * 8) //throw boost::system::system_error
            :_s(addr,port,buffer)
            ,_msg_reader(reader)
        {
            //轉接 回調 函數
            _s.close_bf(boost::bind(&msg_client_t::adapter_close_bf,this,_1,_2));
            _s.recv_bf(boost::bind(&msg_client_t::adapter_recv_bf,this,_1,_2,_3,_4));
            _s.send_bf(boost::bind(&msg_client_t::adapter_send_bf,this,_1,_2,_3));


        }
        msg_client_t& operator=(const msg_client_t&) = delete;
        msg_client_t(const msg_client_t&) = delete;
        ~msg_client_t()
        {
            _s.stop();
            _s.join();
        }

        //啟動 工作 線程
        inline void run()
        {
            _s.run();
        }
        //停止 服務
        inline void stop()
        {
            _s.stop();
        }
        //返回 服務是否 停止
        inline bool stopped()const
        {
            return _s.stopped();
        }
        //返回 工作 線程 數量
        inline std::size_t work_threads()const
        {
            return _s.work_threads();
        }

        //等待 線程 停止 工作
        inline void join()
        {
            _s.join();
        }

        //向 客戶端 發送 隊列 寫入一條 發送 數據
        inline bool push_back_write(socket_spt s,const byte_t* bytes,std::size_t n)
        {
            return _s.push_back_write(s,bytes,n);
        }
        inline bool push_back_write(socket_spt s,bytes_spt buffer)
        {
            return _s.push_back_write(s,buffer);
        }

    protected:
        typedef boost::function<void(msg_client_t*,socket_spt)> close_bft;
        close_bft _close_bf;

        typedef boost::function<void(msg_client_t*,socket_spt,bytes_spt)> recv_bft;
        recv_bft _recv_bf;

        typedef boost::function<void(msg_client_t*,socket_spt,bytes_spt)> send_bft;
        send_bft _send_bf;

    public:
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
        void adapter_close_bf(client_t* s,socket_spt c)
        {
            if(_close_bf)
            {
                _close_bf(this,c);
            }
        }
        void adapter_send_bf(client_t* s,socket_spt c,bytes_spt buffer)
        {
            if(_send_bf)
            {
                _send_bf(this,c,buffer);
            }
        }

        void adapter_recv_bf(client_t* s,socket_spt c,const king::byte_t* buffer,std::size_t n)
        {
            auto& t = c->get_t();
            std::shared_ptr<king::bytes::buffer_t> buf = t.buffer();

            //寫入 數據
            if(buf->write(buffer,n) != n)
            {
                //沒有足夠內存 相應 直接斷開連接
                boost::system::error_code e;
                c->socket().close(e);
                return;
            }

            auto& reader = _msg_reader;

            while(true)
            {
                std::size_t size = buf->size();
                if(t._size == KING_NET_TCP_WAIT_MSG_HEADER)
                {
                    //wait header
                    if(size < reader.header_size())
                    {
                        return;
                    }

                    //read header
                    king::bytes::bytes_t header(reader.header_size());
                    if(header.empty())
                    {
                        //沒有足夠內存 響應 直接斷開連接
                        boost::system::error_code e;
                        c->socket().close(e);
                        return;
                    }
                    buf->copy_to(header.get(),header.size());

                    //get size
                    t._size = reader.get_message_size(header.get(),header.size());
                    if(t._size == KING_NET_TCP_ERROR_MSG)
                    {
                        //協議錯誤 直接斷開連接
                        boost::system::error_code e;
                        c->socket().close(e);
                        return;
                    }
                }

                if(t._size < size)
                {
                    //wait body
                    return;
                }

                //read msg
                try
                {
                    bytes_spt msg = std::make_shared<king::bytes::bytes_t>(t._size);
                    if(msg->empty())
                    {
                        //沒有足夠內存 響應 直接斷開連接
                        boost::system::error_code e;
                        c->socket().close(e);
                        return;
                    }

                    buf->read(msg->get(),msg->size());

                    //通知 客戶
                    if(_recv_bf)
                    {
                        _recv_bf(this,c,msg);
                    }
                }
                catch(const std::bad_alloc&)
                {
                    //沒有足夠內存 響應 直接斷開連接
                    boost::system::error_code e;
                    c->socket().close(e);
                    return;
                }

                //reset flag
                t._size = KING_NET_TCP_WAIT_MSG_HEADER;
            }

        }
    };


};
};
};

#endif // KING_LIB_HEADER_NET_TCP_MSG_CLIENT
