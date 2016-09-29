#include <iostream>
#include "c_dynamic_ktcp.hpp"

#define HEADER_SIZE 6
#define MAX_MSG_SIZE 1024 * 16
#define HEADER_FALG 0x0000044E
std::size_t get_message_size(const ktcp::byte_t* b,std::size_t n);

//#define HEADER_SIZE 5
//std::size_t get_gprs_message_size(const ktcp::byte_t* b,std::size_t n);

#define KING_SHOW_MSG1

bool post_str(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,std::string str);

void on_accept(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket);
void on_close(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket);
void on_recv(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer);
void on_send(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer);

int main()
{
    //加載dll
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();
    funcs.load_library("F:/lib/c++/king-cpp/shared/king/net/tcp/tcpmsg/bin/Release/libtcpmsg.dll");
    if(!funcs)
    {
        std::cout<<"load library error\n";
        return 0;
    }

    //創建 消息解析器
    ktcp::HREADER hReader = funcs.CreateReader(HEADER_SIZE,get_message_size);
    if(!hReader)
    {
        std::cout<<"CreateReader error\n";
        return 0;
    }
    //創建 服務器
    const unsigned short port = 1102;
    ktcp::HSERVER hServer = funcs.CreateServer(port,hReader);

    //銷毀 消息解析 器 服務器創建成功之後 便可銷毀 消息解析器
    funcs.DestoryReader(hReader);
    if(!hServer)
    {
        std::cout<<"CreateServer error\n";
        return 0;
    }
    std::cout<<"work at :"<<port<<"\n";

    //註冊回調事件

    funcs.SetServerOnAccept(hServer,on_accept);
    funcs.SetServerOnClose(hServer,on_close);
    funcs.SetServerOnRecv(hServer,on_recv);
#ifdef KING_SHOW_MSG
    funcs.SetServerOnSend(hServer,on_send);
#endif // KING_SHOW_MSG


    //運行 工作 線程
    funcs.RunServer(hServer);
    std::cout<<"has "<<funcs.GetServerThreads(hServer)<<" work_threads\n";


    //等待 工作 結束
    funcs.JoinServer(hServer);


    //銷毀 服務器
    funcs.DestoryServer(hServer);
    return 0;
}
std::size_t get_message_size(const ktcp::byte_t* b,std::size_t n)
{
    //檢測 包頭 長度
    if(n != HEADER_SIZE)
    {
        return KTCP_NET_TCP_ERROR_MSG;
    }

    //檢查 包頭 標記
    std::uint32_t flag = *(std::uint32_t*)b;
    if(flag != HEADER_FALG)
    {
        return KTCP_NET_TCP_ERROR_MSG;
    }

    //獲取 包長
    std::size_t len = *((std::uint16_t*)(b + 4));
    if(len > MAX_MSG_SIZE)
    {
        return KTCP_NET_TCP_ERROR_MSG;
    }

    return len;
}
std::size_t get_gprs_message_size(const ktcp::byte_t* b,std::size_t n)
{
    //檢測 包頭 長度
    if(n != HEADER_SIZE)
    {
        return KTCP_NET_TCP_ERROR_MSG;
    }

    //檢查 包頭 標記
    if(b[0] != 0xCB ||
		b[1] != 0xCA ||
		b[2] != 0xBB)
	{
		return KTCP_NET_TCP_ERROR_MSG;
	}

    //獲取 包長
    std::size_t len = *((std::uint16_t*)(b + 3));
    //轉 大端 序列
	std::swap(*(ktcp::byte_t*)&len,*(((ktcp::byte_t*)&len)+1));
    if(len > MAX_MSG_SIZE)
    {
        return KTCP_NET_TCP_ERROR_MSG;
    }

    return len + HEADER_SIZE;
}
bool post_str(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,std::string str)
{
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();

    std::size_t size = str.size() + HEADER_SIZE;
    ktcp::HBUFFER hBuffer = funcs.CreateBuffer(size);
    if(!hBuffer)
    {
        //創建 buffer 失敗
        return false;
    }

    ktcp::byte_t* ptr = funcs.GetBufferPtr(hBuffer);
    *(std::uint32_t*)(ptr) = HEADER_FALG;
    *((std::uint16_t*)(ptr + 4)) = size;
    std::copy(str.begin(),str.end(),ptr + HEADER_SIZE);


    if(funcs.ServerWriteBuffer(hServer,hSocket,hBuffer))
    {
        //銷毀 buffer
        funcs.DestoryBuffer(hBuffer);

        //傳入 發送 隊列失敗
        return false;
    }
    //銷毀 buffer
    funcs.DestoryBuffer(hBuffer);
    return true;
}

void on_accept(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket)
{
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();
#ifdef KING_SHOW_MSG
    {
        //創建一份 副本 以便 傳遞到其它地方
        ktcp::HSOCKET hSocketCopy =  funcs.CopySocket(hSocket);
        if(hSocketCopy)
        {
            //不需要 hSocketCopy 時 需要手動 DestorySocket釋放 資源
            funcs.DestorySocket(hSocketCopy);
        }
    }



    std::size_t id = funcs.GetSocketNative(hSocket);
    funcs.SetSocketData(hSocket,id);

    const unsigned short port = funcs.GetSocketRemotePort(hSocket);
    char ip[64] = {0};
    funcs.GetSocketRemoteIp(hSocket,ip,63);
    std::cout<<"one in("<<id<<")\t"<<ip<<":"<<port<<"\n";
#endif // KING_SHOW_MSG
    post_str(hServer,hSocket,"welcome");
    post_str(hServer,hSocket,"this is cerberus's server");
}
void on_close(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket)
{
#ifdef KING_SHOW_MSG
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();

    //don't call any hSocket func other than GetSocketData
    std::size_t id = funcs.GetSocketData(hSocket);

    std::cout<<"one out("<<id<<")\n";
#endif // KING_SHOW_MSG
}

void on_recv(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer)
{
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();

    {
        //創建一份 副本 以便 傳遞到其它地方
        ktcp::HBUFFER hBufferCopy =  funcs.CopyBuffer(hBuffer);
        if(hBufferCopy)
        {
            funcs.DestoryBuffer(hBufferCopy);
        }
    }

    ktcp::byte_t* ptr = funcs.GetBufferPtr(hBuffer);
    std::size_t size = funcs.GetBufferSize(hBuffer);

    std::string str((char*)(ptr + HEADER_SIZE),size - HEADER_SIZE);
#ifdef KING_SHOW_MSG
    std::cout<<"one recv\t"<<str<<"\n";
#endif // KING_SHOW_MSG
    if(str == "i want a job")
    {
        post_str(hServer,hSocket,"what you can do");
    }
    else if(str == "i'm a solider")
    {
        post_str(hServer,hSocket,"you are cerberus soldier now");
    }
    else if(str == "you are die")
    {
		funcs.StopServer(hServer);
    }
}
void on_send(ktcp::HSERVER hServer,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer)
{
    ktcp::funcs_t& funcs = ktcp::funcs_t::instance();

    {
        //創建一份 副本 以便 傳遞到其它地方
        ktcp::HBUFFER hBufferCopy =  funcs.CopyBuffer(hBuffer);
        if(hBufferCopy)
        {
            //不需要 hBufferCopy 時 需要手動 DestorySocket釋放 資源
            funcs.DestoryBuffer(hBufferCopy);
        }
    }



    ktcp::byte_t* ptr = funcs.GetBufferPtr(hBuffer);
    std::size_t size = funcs.GetBufferSize(hBuffer);

    std::string str((char*)(ptr + HEADER_SIZE),size - HEADER_SIZE);
    std::cout<<"one send\t"<<str<<"\n";
}
