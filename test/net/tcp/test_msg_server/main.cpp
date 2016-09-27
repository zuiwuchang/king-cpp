#include <iostream>
#include <atomic>
#include <king/net/tcp/msg_server.hpp>

typedef std::size_t user_t; //和socket 關聯的 用戶自定義 數據
typedef king::net::tcp::msg_server_t<user_t> msg_server_t;
typedef msg_server_t::socket_spt socket_spt;

void on_accet(msg_server_t* s,socket_spt c);
void on_close(msg_server_t* s,socket_spt c);
void on_recv(msg_server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer);
void on_send(msg_server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer);

void post_str(msg_server_t* s,socket_spt c,std::string str);

#define HEADER_SIZE 6
#define MAX_MSG_SIZE 1024 * 16
#define HEADER_FALG 0x0000044E
std::size_t get_message_size(const king::byte_t* b,std::size_t n);


std::size_t get_id();
int main()
{
    unsigned short port = 1102;
    try
    {

        //創建 消息解析器
        king::net::tcp::msg_reader_t reader(HEADER_SIZE,boost::bind(get_message_size,_1,_2));

        //創建 服務器
        msg_server_t s(port,reader);
        std::cout<<"work at :"<<port<<"\n";

        //註冊 回調
        s.accept_bf(boost::bind(on_accet,_1,_2));
        s.close_bf(boost::bind(on_close,_1,_2));
        s.recv_bf(boost::bind(on_recv,_1,_2,_3));
        //s.send_bf(boost::bind(on_send,_1,_2,_3));   //只有 push_back_write 是數據會回調次函數


        //運行 工作 線程
        s.run();
        std::cout<<"has "<<s.work_threads()<<" work_threads\n";


        //等待 工作 結束
        s.join();
    }
    catch(const boost::system::system_error& e)
    {
        std::cout<<e.what()<<"\n";
    }

    return 0;
}
void on_accet(msg_server_t* s,socket_spt c)
{
    std::size_t id = get_id();
    c->get_t().get_t() = id;

    std::cout<<"one in("<<id<<")\t"
        <<c->socket().remote_endpoint().address().to_string()
        <<":"
        <<c->socket().remote_endpoint().port()
        <<"\n";

        post_str(s,c,"welcome");
        post_str(s,c,"this is cerberus's server");

}
void on_close(msg_server_t* s,socket_spt c)
{
    //don't call any c's function at here
    std::size_t id = c->get_t().get_t();
    std::cout<<"one out("<<id<<")\n";
}
void on_recv(msg_server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)(buffer->get() + HEADER_SIZE),buffer->size() - HEADER_SIZE);
    //std::cout<<"one recv\t"<<str<<"\n";

    if(str == "i want a job")
    {
        post_str(s,c,"what you can do");
    }
    else if(str == "i'm a solider")
    {
        post_str(s,c,"you are cerberus soldier now");
    }
}
void on_send(msg_server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)(buffer->get() + HEADER_SIZE),buffer->size() - HEADER_SIZE);
    std::cout<<"one send\t"<<str<<"\n";
}

void post_str(msg_server_t* s,socket_spt c,std::string str)
{
    try
    {
        std::size_t size = str.size() + HEADER_SIZE;
        king::net::tcp::bytes_spt buffer = std::make_shared<king::bytes::bytes_t>(size);
        if(buffer->empty())
        {
            //創建 失敗
            return;
        }

        *(std::uint32_t*)(buffer->get()) = HEADER_FALG;
        *((std::uint16_t*)(buffer->get() + 4)) = size;

        std::copy(str.begin(),str.end(),buffer->get() + HEADER_SIZE);


        s->push_back_write(c,buffer);
    }
    catch(const std::bad_alloc&)
    {
    }
}

std::size_t get_message_size(const king::byte_t* b,std::size_t n)
{
    //檢測 包頭 長度
    if(n != HEADER_SIZE)
    {
        return KING_NET_TCP_ERROR_MSG;
    }

    //檢查 包頭 標記
    std::uint32_t flag = *(std::uint32_t*)b;
    if(flag != HEADER_FALG)
    {
        return KING_NET_TCP_ERROR_MSG;
    }

    //獲取 包長
    std::size_t len = *((std::uint16_t*)(b + 4));
    if(len > MAX_MSG_SIZE)
    {
        return KING_NET_TCP_ERROR_MSG;
    }

    return len;
}

std::size_t get_id()
{
    static std::atomic<std::size_t> id;
    return ++id;
}
