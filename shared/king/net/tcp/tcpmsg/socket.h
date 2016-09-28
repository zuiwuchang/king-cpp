#ifndef SOCKET_H_INCLUDED
#define SOCKET_H_INCLUDED

#include <memory>

#include <king/net/tcp/msg_reader.hpp>
#include <king/net/tcp/type.hpp>


#include "configure.h"

typedef king::net::tcp::msg_data<std::size_t,1024*8> user_t;
typedef std::shared_ptr<king::net::tcp::socket_t<user_t>> KTcp_SOCKET;
typedef KTcp_SOCKET* KTcp_HSOCKET;

//創建 socket 的一個副本 失敗 返回 NULL
//需要手動使用 DestorySocket 銷毀副本
KING_C_API
KTcp_HSOCKET KING_C_CALL KTcp_CopySocket(KTcp_HSOCKET hSocket);

//銷毀 socket
KING_C_API
void KING_C_CALL KTcp_DestorySocket(KTcp_HSOCKET hSocket);

//斷開 socket 連接
KING_C_API
void KING_C_CALL KTcp_CloseSocket(KTcp_HSOCKET hSocket);


//設置 socket 關聯數據
KING_C_API
void KING_C_CALL KTcp_SetSocketData(KTcp_HSOCKET hSocket,const std::size_t data);

//獲取 socket 關聯數據
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketData(KTcp_HSOCKET hSocket);

//獲取 socket 本地 句柄
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketNative(KTcp_HSOCKET hSocket);

//返回 本地 端口
KING_C_API
unsigned short KING_C_CALL KTcp_GetSocketLocalPort(KTcp_HSOCKET hSocket);

//返回 本地 字符串長度 不包含 0
//如果 buf 足夠 將 ip copy到 buf中
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketLocalIp(KTcp_HSOCKET hSocket,char* buf,std::size_t n);

//返回 遠端 端口
KING_C_API
unsigned short KING_C_CALL KTcp_GetSocketRemotePort(KTcp_HSOCKET hSocket);

//返回 遠端 字符串長度 不包含 0
//如果 buf 足夠 將 ip copy到 buf中
KING_C_API
std::size_t KING_C_CALL KTcp_GetSocketRemoteIp(KTcp_HSOCKET hSocket,char* buf,std::size_t n);


#endif // SOCKET_H_INCLUDED
