#include "socket.h"

KING_C_API
KTcp_HSOCKET KING_C_CALL KTcp_CopySocket(KTcp_HSOCKET hSocket)
{
    KTcp_HSOCKET p = NULL;
    try
    {
        p = new KTcp_SOCKET();
        *p = *hSocket;
    }
    catch(const std::bad_alloc&)
    {

    }
    return p;

}

KING_C_API
void KING_C_CALL KTcp_DestorySocket(KTcp_HSOCKET hSocket)
{
    delete hSocket;
}

//斷開 socket 連接
KING_C_API
void KING_C_CALL KTcp_CloseSocket(KTcp_HSOCKET hSocket)
{
    if((*hSocket)->socket().is_open())
    {
        boost::system::error_code e;
        (*hSocket)->socket().close(e);
    }

}

//設置 socket 關聯數據
KING_C_API
void KING_C_CALL KTcp_SetSocketData(KTcp_HSOCKET hSocket,const std::size_t data)
{
    hSocket->get()->get_t().get_t() = data;
}

//獲取 socket 關聯數據
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketData(KTcp_HSOCKET hSocket)
{
    return hSocket->get()->get_t().get_t();
}

//獲取 socket 本地 句柄
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketNative(KTcp_HSOCKET hSocket)
{
    return (*hSocket)->socket().native();
}

//返回 本地 端口
KING_C_API
unsigned short KING_C_CALL KTcp_GetSocketLocalPort(KTcp_HSOCKET hSocket)
{
    boost::system::error_code e;
    auto endpoint = (*hSocket)->socket().local_endpoint(e);
    if(e)
    {
        return 0;
    }

    return endpoint.port();
}
//返回 本地 字符串長度 不包含 0
//如果 buf 足夠 將 ip copy到 buf中
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketLocalIp(KTcp_HSOCKET hSocket,char* buf,std::size_t n)
{
    boost::system::error_code e;
    auto endpoint = (*hSocket)->socket().local_endpoint(e);
    if(e)
    {
        return 0;
    }

    std::string str = endpoint.address().to_string(e);
    if(e)
    {
        return 0;
    }
    std::size_t size = str.size();
    if(buf && n >= size)
    {
        std::copy(str.begin(),str.end(),buf);
    }

    return size;
}

//返回 遠端 端口
KING_C_API
unsigned short KING_C_CALL KTcp_GetSocketRemotePort(KTcp_HSOCKET hSocket)
{
    boost::system::error_code e;
    auto endpoint = (*hSocket)->socket().remote_endpoint(e);
    if(e)
    {
        return 0;
    }

    return endpoint.port();
}

//返回 遠端 字符串長度 不包含 0
//如果 buf 足夠 將 ip copy到 buf中
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketRemoteIp(KTcp_HSOCKET hSocket,char* buf,std::size_t n)
{
    boost::system::error_code e;
    auto endpoint = (*hSocket)->socket().remote_endpoint(e);
    if(e)
    {
        return 0;
    }

    std::string str = endpoint.address().to_string(e);
    if(e)
    {
        return 0;
    }
    std::size_t size = str.size();
    if(buf && n >= size)
    {
        std::copy(str.begin(),str.end(),buf);
    }

    return size;
}
