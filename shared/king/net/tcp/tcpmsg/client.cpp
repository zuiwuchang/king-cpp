#include "client.h"

//創建 客戶端
KING_C_API
KTcp_HCLIENT KING_C_CALL KTcp_CreateClient(const char* addr,const unsigned short port,KTcp_HREADER hReader)
{
    KTcp_HCLIENT hClient = NULL;
    try
    {
        hClient = new KTcp_CLIENT(addr,port,*hReader);
    }
    catch(const boost::system::system_error&)
    {

    }
    catch(const std::bad_alloc&)
    {

    }
    return hClient;
}

//銷毀 客戶端
KING_C_API
void KING_C_CALL KTcp_DestoryClient(KTcp_HCLIENT hClient)
{
    delete hClient;
}

//啟動 工作 線程
KING_C_API
void KING_C_CALL KTcp_RunClient(KTcp_HCLIENT hClient)
{
    hClient->run();
}

//停止 服務
KING_C_API
void KING_C_CALL KTcp_StopClient(KTcp_HCLIENT hClient)
{
    hClient->stop();
}

//返回 服務是否 停止
KING_C_API
BOOL KING_C_CALL KTcp_IsClientStopped(KTcp_HCLIENT hClient)
{
    return hClient->stopped();
}

//返回 工作 線程 數量
KING_C_API
std::size_t KING_C_CALL KTcp_GetClientThreads(KTcp_HCLIENT hClient)
{
    return hClient->work_threads();
}

//等待 線程 停止 工作
KING_C_API
void KING_C_CALL KTcp_JoinClient(KTcp_HCLIENT hClient)
{
    hClient->join();
}

//向 客戶端 發送 隊列 寫入一條 發送 數據
KING_C_API
BOOL KING_C_CALL KTcp_ClientWriteBytes(KTcp_HCLIENT hClient,KTcp_HSOCKET hSocket,const king::byte_t* bytes,std::size_t n)
{
    if(hClient->push_back_write(*hSocket,bytes,n))
    {
        return TRUE;
    }
    return FALSE;
}

KING_C_API
BOOL KING_C_CALL KTcp_ClientWriteBuffer(KTcp_HCLIENT hClient,KTcp_HSOCKET hSocket,KTcp_HBUFFER hBuffer)
{
    if(hClient->push_back_write(*hSocket,*hBuffer))
    {
        return TRUE;
    }

    return FALSE;
}

void __ktcp__client_close(KTcp_FuncClientOnClose func,KTcp_HCLIENT hClient,KTcp_SOCKET s)
{
    func(hClient,&s);
}
//註冊 斷開 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnClose(KTcp_HCLIENT hClient,KTcp_FuncClientOnClose func)
{
    hClient->close_bf(boost::bind(__ktcp__client_close,func,_1,_2));
}

void __ktcp__client_recv(KTcp_FuncClientOnRecv func,KTcp_HCLIENT hClient,KTcp_SOCKET s,KTcp_BUFFER b)
{
    func(hClient,&s,&b);
}
//註冊 recv 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnRecv(KTcp_HCLIENT hClient,KTcp_FuncClientOnRecv func)
{
    hClient->recv_bf(boost::bind(__ktcp__client_recv,func,_1,_2,_3));
}

void __ktcp__client_send(KTcp_FuncClientOnSend func,KTcp_HCLIENT hClient,KTcp_SOCKET s,KTcp_BUFFER b)
{
    func(hClient,&s,&b);
}
//註冊 send 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnSend(KTcp_HCLIENT hClient,KTcp_FuncClientOnSend func)
{
    hClient->send_bf(boost::bind(__ktcp__client_send,func,_1,_2,_3));
}
