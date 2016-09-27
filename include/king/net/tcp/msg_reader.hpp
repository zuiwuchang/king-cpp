//消息解析器
#ifndef KING_LIB_HEADER_NET_TCP_MSG_READER
#define KING_LIB_HEADER_NET_TCP_MSG_READER

#include <king/bytes/buffer.hpp>

#include <boost/function.hpp>


namespace king
{
namespace net
{
namespace tcp
{

#define KING_NET_TCP_ERROR_MSG          ((std::size_t)-1)
#define KING_NET_TCP_WAIT_MSG_HEADER    ((std::size_t)-2)
    class msg_reader_t
    {
    protected:
        typedef king::byte_t byte_t;
        std::size_t _header_size;
    public:
        typedef boost::function<std::size_t(const byte_t*,std::size_t)> get_message_size_bft;
    protected:
        get_message_size_bft _get_message_size_bf;
    public:
        explicit msg_reader_t(const std::size_t header_size,get_message_size_bft get_message_size_bf)
            :_header_size(header_size),
            _get_message_size_bf(get_message_size_bf)
        {

        }

        inline std::size_t header_size()const
        {
            return _header_size;
        }

        //傳入 header 返回 msg 長度
        //如果 返回 KING_NET_TCP_ERROR_MSG 將 斷開 tcp
        inline std::size_t get_message_size(const byte_t* b,std::size_t n)const
        {
            return _get_message_size_bf(b,n);
        }

    };

    template<typename T,std::size_t N/*解析器 分片大小*/>
    class msg_data
    {
    protected:
        T _user;
        std::shared_ptr<king::bytes::buffer_t> _buffer;
    public:
        std::size_t _size = KING_NET_TCP_WAIT_MSG_HEADER; // 消息 長度

        msg_data()
        {
            _buffer = std::make_shared<king::bytes::buffer_t>();
        }
        inline T& get_t()
        {
            return _user;
        }
        inline std::shared_ptr<king::bytes::buffer_t> buffer()const
        {
            return _buffer;
        }

    };

};
};
};

#endif // KING_LIB_HEADER_NET_TCP_MSG_READER
