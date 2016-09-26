#include <iostream>
#include <king/net/tcp/server.hpp>

void on_accet(king::net::tcp::server_t* s,king::net::tcp::socket_spt c);
void on_close(king::net::tcp::server_t* s,king::net::tcp::socket_spt c);
void on_recv(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,const king::byte_t* bytes,std::size_t n);
void on_send(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,king::net::tcp::bytes_spt buffer);

void post_str(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,std::string str);
int main()
{
    unsigned short port = 1102;
    try
    {
        //創建 服務器
        king::net::tcp::server_t s(port);
        std::cout<<"work at :"<<port<<"\n";

        //註冊 回調
        s.accept_bf(boost::bind(on_accet,_1,_2));
        s.close_bf(boost::bind(on_close,_1,_2));
        s.recv_bf(boost::bind(on_recv,_1,_2,_3,_4));
        s.send_bf(boost::bind(on_send,_1,_2,_3));


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
void on_accet(king::net::tcp::server_t* s,king::net::tcp::socket_spt c)
{
    std::cout<<"one in\t"
        <<c->socket().remote_endpoint().address().to_string()
        <<":"
        <<c->socket().remote_endpoint().port()
        <<"\n";

        post_str(s,c,"welcome");
        post_str(s,c,"this is cerberus's server");

}
void on_close(king::net::tcp::server_t* s,king::net::tcp::socket_spt c)
{
    //don't call any c's function at here
    std::cout<<"one out\n";
}
void on_recv(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,const king::byte_t* bytes,std::size_t n)
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
void on_send(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,king::net::tcp::bytes_spt buffer)
{
    std::string str((const char*)buffer->get(),buffer->size());
    std::cout<<"one send\t"<<str<<"\n";
}

void post_str(king::net::tcp::server_t* s,king::net::tcp::socket_spt c,std::string str)
{
    const king::byte_t* b = (const king::byte_t*)str.data();
    std::size_t n = str.size();
    s->push_back_write(c,b,n);
}
