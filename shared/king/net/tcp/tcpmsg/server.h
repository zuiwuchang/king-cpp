#ifndef SERVER_H_INCLUDED
#define SERVER_H_INCLUDED

#include <king/net/tcp/msg_server.hpp>

#include "configure.h"
#include "reader.h"
#include "socket.h"
#include "buffer.h"


//服務器 句柄
typedef king::net::tcp::msg_server_t<std::size_t> KTcp_SERVER;
typedef KTcp_SERVER* KTcp_HSERVER;

typedef void (KING_C_CALL *KTcp_FuncServerOnAccept)(KTcp_HSERVER,KTcp_HSOCKET);
typedef void (KING_C_CALL *KTcp_FuncServerOnClose)(KTcp_HSERVER,KTcp_HSOCKET);
typedef void (KING_C_CALL *KTcp_FuncServerOnRecv)(KTcp_HSERVER,KTcp_HSOCKET,KTcp_HBUFFER);
typedef void (KING_C_CALL *KTcp_FuncServerOnSend)(KTcp_HSERVER,KTcp_HSOCKET,KTcp_HBUFFER);

//創建 服務器
KING_C_API
KTcp_HSERVER KING_C_CALL KTcp_CreateServer(const unsigned short port,KTcp_HREADER hReader);

//銷毀 服務器
KING_C_API
void KING_C_CALL KTcp_DestoryServer(KTcp_HSERVER hServer);


//啟動 工作 線程
KING_C_API
void KING_C_CALL KTcp_RunServer(KTcp_HSERVER hServer);

//停止 服務
KING_C_API
void KING_C_CALL KTcp_StopServer(KTcp_HSERVER hServer);

//返回 服務是否 停止
KING_C_API
BOOL KING_C_CALL KTcp_IsServerStopped(KTcp_HSERVER hServer);

//返回 工作 線程 數量
KING_C_API
std::size_t KING_C_CALL KTcp_GetServerThreads(KTcp_HSERVER hServer);

//等待 線程 停止 工作
KING_C_API
void KING_C_CALL KTcp_JoinServer(KTcp_HSERVER hServer);

//向 客戶端 發送 隊列 寫入一條 發送 數據
KING_C_API
BOOL KING_C_CALL KTcp_ServerWriteBytes(KTcp_HSERVER hServer,KTcp_HSOCKET hSocket,const king::byte_t* bytes,std::size_t n);

KING_C_API
BOOL KING_C_CALL KTcp_ServerWriteBuffer(KTcp_HSERVER hServer,KTcp_HSOCKET hSocket,KTcp_HBUFFER hBuffer);



//註冊 連接 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnAccept(KTcp_HSERVER hServer,KTcp_FuncServerOnAccept func);

//註冊 斷開 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnClose(KTcp_HSERVER hServer,KTcp_FuncServerOnClose func);

//註冊 recv 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnRecv(KTcp_HSERVER hServer,KTcp_FuncServerOnRecv func);

//註冊 send 回調
KING_C_API
void KING_C_CALL KTcp_SetServerOnSend(KTcp_HSERVER hServer,KTcp_FuncServerOnSend func);


#endif // SERVER_H_INCLUDED
