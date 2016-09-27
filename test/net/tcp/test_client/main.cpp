#include <iostream>
#include <king/net/tcp/client.hpp>
using namespace std;

void on_close(king::net::tcp::client_t* s,king::net::tcp::socket_spt c);
void on_recv(king::net::tcp::client_t* s,king::net::tcp::socket_spt c,const king::byte_t* bytes,std::size_t n);
void on_send(king::net::tcp::client_t* s,king::net::tcp::socket_spt c,king::net::tcp::bytes_spt buffer);

void post_str(king::net::tcp::client_t* s,std::string str);

int main()
{
    const std::string addr = "127.0.0.1";
    unsigned short port = 1102;
    try
    {
        //創建 服務器
        king::net::tcp::client_t c(addr,port);
        std::cout<<"connect at "<<addr<<":"<<port<<"\n";

        //註冊 回調
        c.close_bf(boost::bind(on_close,_1,_2));
        c.recv_bf(boost::bind(on_recv,_1,_2,_3,_4));
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

void on_close(king::net::tcp::client_t* s,king::net::tcp::socket_spt c)
{
    //don't call any c's function at here
    std::cout<<"disconnect\n";
}
void on_recv(king::net::tcp::client_t* s,king::net::tcp::socket_spt c,const king::byte_t* bytes,std::size_t n)
{
    std::string str((const char*)bytes,n);
    std::cout<<"one recv\t"<<str<<"\n";

    if(str == "this is cerberus's server")
    {
        post_str(s,"i want a job");
    }
    else if(str == "what you can do")
    {
        post_str(s,"i'm a solider");
    }
    else if(str == "you are cerberus soldier now")
    {
        s->stop();
    }
}
void on_send(king::net::tcp::client_t* s,king::net::tcp::socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)buffer->get(),buffer->size());
    std::cout<<"one send\t"<<str<<"\n";
}

void post_str(king::net::tcp::client_t* s,std::string str)
{
    const king::byte_t* b = (const king::byte_t*)str.data();
    std::size_t n = str.size();
    s->push_back_write(b,n);
}
