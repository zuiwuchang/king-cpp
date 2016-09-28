#include <iostream>

#include "../test_server/c_dynamic_ktcp.hpp"

#define HEADER_SIZE 6
#define MAX_MSG_SIZE 1024 * 16
#define HEADER_FALG 0x0000044E
std::size_t get_message_size(const ktcp::byte_t* b,std::size_t n);

bool post_str(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,std::string str);

void on_close(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket);
void on_recv(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer);
void on_send(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer);
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

    //創建 客戶端
    const char* addr = "127.0.0.1";
    const unsigned short port = 1102;
    ktcp::HCLIENT hClient = funcs.CreateClient(addr,port,hReader);

    //銷毀 消息解析 器 客戶端創建成功之後 便可銷毀 消息解析器
    funcs.DestoryReader(hReader);
    if(!hClient)
    {
        std::cout<<"CreateClient error\n";
        return 0;
    }
    std::cout<<"connect succes\n";

    //註冊回調事件
    funcs.SetClientOnClose(hClient,on_close);
    funcs.SetClientOnRecv(hClient,on_recv);
    funcs.SetClientOnSend(hClient,on_send);


    //運行 工作 線程
    funcs.RunClient(hClient);
    std::cout<<"has "<<funcs.GetClientThreads(hClient)<<" work_threads\n";


    //等待 工作 結束
    funcs.JoinClient(hClient);


    //銷毀 客戶端
    funcs.DestoryClient(hClient);
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

bool post_str(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,std::string str)
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


    if(funcs.ClientWriteBuffer(hClient,hSocket,hBuffer))
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
void on_close(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket)
{
    //don't call any hSocket func other than GetSocketData
    std::cout<<"disconnect\n";
}
void on_recv(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer)
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
    std::cout<<"one recv\t"<<str<<"\n";

    if(str == "this is cerberus's server")
    {
        post_str(hClient,hSocket,"i want a job");
    }
    else if(str == "what you can do")
    {
        post_str(hClient,hSocket,"i'm a solider");
    }
    else if(str == "you are cerberus soldier now")
    {
        funcs.StopClient(hClient);
    }
}
void on_send(ktcp::HCLIENT hClient,ktcp::HSOCKET hSocket,ktcp::HBUFFER hBuffer)
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
