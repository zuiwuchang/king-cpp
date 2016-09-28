#include "server.h"

//創建 服務器
KING_C_API
KTcp_HSERVER KING_C_CALL KTcp_CreateServer(const unsigned short port,KTcp_HREADER hReader)
{
    KTcp_HSERVER hServer = NULL;
    try
    {
        hServer = new KTcp_SERVER(port,*hReader);
    }
    catch(const boost::system::system_error&)
    {

    }
    catch(const std::bad_alloc&)
    {

    }
    return hServer;
}

//銷毀 服務器
KING_C_API
void KING_C_CALL KTcp_DestoryServer(KTcp_HSERVER hServer)
{
    delete hServer;
}


//啟動 工作 線程
KING_C_API
void KING_C_CALL KTcp_RunServer(KTcp_HSERVER hServer)
{
    hServer->run();
}

//停止 服務
KING_C_API
void KING_C_CALL KTcp_StopServer(KTcp_HSERVER hServer)
{
    hServer->stop();
}

//返回 服務是否 停止
KING_C_API
BOOL KING_C_CALL KTcp_IsServerStopped(KTcp_HSERVER hServer)
{
    if(hServer->stopped())
    {
        return TRUE;
    }
    return FALSE;
}

//返回 工作 線程 數量
KING_C_API
std::size_t KING_C_CALL KTcp_GetServerThreads(KTcp_HSERVER hServer)
{
    return hServer->work_threads();
}

//等待 線程 停止 工作
KING_C_API
void KING_C_CALL KTcp_JoinServer(KTcp_HSERVER hServer)
{
    hServer->join();
}


//向 客戶端 發送 隊列 寫入一條 發送 數據
KING_C_API
BOOL KING_C_CALL KTcp_ServerWriteBytes(KTcp_HSERVER hServer,KTcp_HSOCKET hSocket,const king::byte_t* bytes,std::size_t n)
{
    if(hServer->push_back_write(*hSocket,bytes,n))
    {
        return TRUE;
    }
    return FALSE;
}

KING_C_API
BOOL KING_C_CALL KTcp_ServerWriteBuffer(KTcp_HSERVER hServer,KTcp_HSOCKET hSocket,KTcp_HBUFFER hBuffer)
{
    if(hServer->push_back_write(*hSocket,*hBuffer))
    {
        return TRUE;
    }

    return FALSE;
}

void __dtcp__server_accept(KTcp_FuncServerOnAccept func,KTcp_HSERVER hServer,KTcp_SOCKET s)
{
    func(hServer,&s);
}
KING_C_API
void KING_C_CALL KTcp_SetServerOnAccept(KTcp_HSERVER hServer,KTcp_FuncServerOnAccept func)
{
    hServer->accept_bf(boost::bind(__dtcp__server_accept,func,_1,_2));

}

void __dtcp__server_close(KTcp_FuncServerOnClose func,KTcp_HSERVER hServer,KTcp_SOCKET s)
{
    func(hServer,&s);
}
//註冊 斷開 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnClose(KTcp_HSERVER hServer,KTcp_FuncServerOnClose func)
{
    hServer->close_bf(boost::bind(__dtcp__server_close,func,_1,_2));
}


void __dtcp__server_recv(KTcp_FuncServerOnRecv func,KTcp_HSERVER hServer,KTcp_SOCKET s,KTcp_BUFFER b)
{
    func(hServer,&s,&b);
}
//註冊 recv 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnRecv(KTcp_HSERVER hServer,KTcp_FuncServerOnRecv func)
{
    hServer->recv_bf(boost::bind(__dtcp__server_recv,func,_1,_2,_3));
}

void __dtcp__server_send(KTcp_FuncServerOnSend func,KTcp_HSERVER hServer,KTcp_SOCKET s,KTcp_BUFFER b)
{
    func(hServer,&s,&b);
}
//註冊 send 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnSend(KTcp_HSERVER hServer,KTcp_FuncServerOnSend func)
{
    hServer->send_bf(boost::bind(__dtcp__server_send,func,_1,_2,_3));
}
