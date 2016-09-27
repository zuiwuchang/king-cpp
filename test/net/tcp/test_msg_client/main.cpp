#include <iostream>
#include <king/net/tcp/msg_client.hpp>

typedef std::size_t user_t; //和socket 關聯的 用戶自定義 數據
typedef king::net::tcp::msg_client_t<user_t> msg_client_t;
typedef msg_client_t::socket_spt socket_spt;


void on_close(msg_client_t* s,socket_spt c);
void on_recv(msg_client_t* s,socket_spt c,king::net::tcp::bytes_spt buffer);
void on_send(msg_client_t* s,socket_spt c,king::net::tcp::bytes_spt buffer);

void post_str(msg_client_t* s,socket_spt c,std::string str);

#define HEADER_SIZE 6
#define MAX_MSG_SIZE 1024 * 16
#define HEADER_FALG 0x0000044E
std::size_t get_message_size(const king::byte_t* b,std::size_t n);

int main()
{
    const std::string addr = "127.0.0.1";
    unsigned short port = 1102;
    try
    {
        //創建 消息解析器
        king::net::tcp::msg_reader_t reader(HEADER_SIZE,boost::bind(get_message_size,_1,_2));

        //創建 服務器
        msg_client_t c(addr,port,reader);
        std::cout<<"connect at "<<addr<<":"<<port<<"\n";

        //註冊 回調
        c.close_bf(boost::bind(on_close,_1,_2));
        c.recv_bf(boost::bind(on_recv,_1,_2,_3));
        c.send_bf(boost::bind(on_send,_1,_2,_3)); //只有 push_back_write 是數據會回調次函數


        //運行 工作 線程
        c.run();
        std::cout<<"has "<<c.work_threads()<<" work_threads\n";


        //等待 工作 結束
        c.join();
    }
    catch(const boost::system::system_error& e)
    {
        std::cout<<e.what()<<"\n";
    }
    catch(const std::bad_alloc)
    {
        std::cout<<"bad alloc create recv buffer"<<"\n";
    }

    return 0;
}


void on_close(msg_client_t* s,socket_spt c)
{
    //don't call any c's function at here
    std::cout<<"disconnect\n";
}
void on_recv(msg_client_t* s,socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)(buffer->get() + HEADER_SIZE),buffer->size() - HEADER_SIZE);
    std::cout<<"one recv\t"<<str<<"\n";

    if(str == "this is cerberus's server")
    {
        post_str(s,c,"i want a job");
    }
    else if(str == "what you can do")
    {
        post_str(s,c,"i'm a solider");
    }
    else if(str == "you are cerberus soldier now")
    {
        s->stop();
    }
}
void on_send(msg_client_t* s,socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)(buffer->get() + HEADER_SIZE),buffer->size() - HEADER_SIZE);
    std::cout<<"one send\t"<<str<<"\n";
}

void post_str(msg_client_t* s,socket_spt c,std::string str)
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
