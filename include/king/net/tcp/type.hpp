//使用 boost asio 製作的 一個 tcp 庫
//依賴 boost::asio boost::thread
#ifndef KING_LIB_HEADER_NET_TCP_TYPE
#define KING_LIB_HEADER_NET_TCP_TYPE

#include <king/bytes/type.hpp>

#include <boost/asio.hpp>
#include <boost/thread.hpp>


#include <memory>
#include <list>
namespace king
{
namespace net
{
namespace tcp
{
    typedef boost::asio::io_service io_service_t;
    typedef boost::asio::ip::tcp::acceptor acceptor_t;

    typedef std::shared_ptr<king::bytes::bytes_t> bytes_spt;

    //對 boost socket 結構的 擴展
    //增加 post_write 將數據 寫入 一個 隊列
    //以使 多次 異步 async_write_some 之間 不會 亂順序
    //盡量 不要將 post_write 和 async_write_some 混用
    template<typename T>
    class socket_t
    {
    protected:
        //boost socket
        typedef boost::asio::ip::tcp::socket socket_bt;
        socket_bt _s;

        //用戶 自定義關聯 數據
        T _user;
    public:
        explicit socket_t(io_service_t& io_s):_s(io_s)
        {

        }
        //返回 boost socket 的引用
        socket_bt& socket()
        {
            return _s;
        }
        T& get_t()
        {
            return _user;
        }

        //等發 數據 列表
        std::list<bytes_spt> _datas;
        //同步 對象
        boost::mutex _mutex;

        //是否正在 等待上次 write
        bool wait = false;

    };

};
};
};

#endif // KING_LIB_HEADER_NET_TCP_TYPE
