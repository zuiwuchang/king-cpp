#include <iostream>
#include <atomic>
#include <king/net/tcp/server.hpp>

typedef std::size_t user_t; //和socket 關聯的 用戶自定義 數據
typedef king::net::tcp::server_t<user_t> server_t;
typedef std::shared_ptr<king::net::tcp::socket_t<user_t>> socket_spt;

void on_accet(server_t* s,socket_spt c);
void on_close(server_t* s,socket_spt c);
void on_recv(server_t* s,socket_spt c,const king::byte_t* bytes,std::size_t n);
void on_send(server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer);

void post_str(server_t* s,socket_spt c,std::string str);

std::size_t get_id();
int main()
{
    unsigned short port = 1102;
    try
    {
        //創建 服務器
        server_t s(port);
        std::cout<<"work at :"<<port<<"\n";

        //註冊 回調
        s.accept_bf(boost::bind(on_accet,_1,_2));
        s.close_bf(boost::bind(on_close,_1,_2));
        s.recv_bf(boost::bind(on_recv,_1,_2,_3,_4));
        s.send_bf(boost::bind(on_send,_1,_2,_3));   //只有 push_back_write 是數據會回調次函數


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

void on_accet(server_t* s,socket_spt c)
{
    std::size_t id = get_id();
    c->get_t() = id;
    std::cout<<"one in("<<id<<")\t"
        <<c->socket().remote_endpoint().address().to_string()
        <<":"
        <<c->socket().remote_endpoint().port()
        <<"\n";

        post_str(s,c,"welcome");
        post_str(s,c,"this is cerberus's server");

}
void on_close(server_t* s,socket_spt c)
{
    //don't call any c's function at here
    std::size_t id = c->get_t();
    std::cout<<"one out("<<id<<")\n";
}
void on_recv(server_t* s,socket_spt c,const king::byte_t* bytes,std::size_t n)
{
    std::string str((const char*)bytes,n);
    std::cout<<"one recv\t"<<str<<"\n";

    if(str == "i want a job")
    {
        post_str(s,c,"what you can do");
    }
    else if(str == "i'm a solider")
    {
        post_str(s,c,"you are cerberus soldier now");
    }
}
void on_send(server_t* s,socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)buffer->get(),buffer->size());
    std::cout<<"one send\t"<<str<<"\n";
}

void post_str(server_t* s,socket_spt c,std::string str)
{
    const king::byte_t* b = (const king::byte_t*)str.data();
    std::size_t n = str.size();
    s->push_back_write(c,b,n);
}

std::size_t get_id()
{
    static std::atomic<std::size_t> id;
    return ++id;
}
