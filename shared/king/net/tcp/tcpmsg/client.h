#ifndef CLIENT_H_INCLUDED
#define CLIENT_H_INCLUDED


#include <king/net/tcp/msg_client.hpp>

#include "configure.h"
#include "reader.h"
#include "socket.h"
#include "buffer.h"


//客戶端 句柄
typedef king::net::tcp::msg_client_t<std::size_t> KTcp_CLIENT;
typedef KTcp_CLIENT* KTcp_HCLIENT;


typedef void (KING_C_CALL *KTcp_FuncClientOnClose)(KTcp_HCLIENT,KTcp_HSOCKET);
typedef void (KING_C_CALL *KTcp_FuncClientOnRecv)(KTcp_HCLIENT,KTcp_HSOCKET,KTcp_HBUFFER);
typedef void (KING_C_CALL *KTcp_FuncClientOnSend)(KTcp_HCLIENT,KTcp_HSOCKET,KTcp_HBUFFER);


//創建 客戶端
KING_C_API
KTcp_HCLIENT KING_C_CALL KTcp_CreateClient(const char* addr,const unsigned short port,KTcp_HREADER hReader);

//銷毀 客戶端
KING_C_API
void KING_C_CALL KTcp_DestoryClient(KTcp_HCLIENT hClient);


//啟動 工作 線程
KING_C_API
void KING_C_CALL KTcp_RunClient(KTcp_HCLIENT hClient);

//停止 服務
KING_C_API
void KING_C_CALL KTcp_StopClient(KTcp_HCLIENT hClient);

//返回 服務是否 停止
KING_C_API
BOOL KING_C_CALL KTcp_IsClientStopped(KTcp_HCLIENT hClient);

//返回 工作 線程 數量
KING_C_API
std::size_t KING_C_CALL KTcp_GetClientThreads(KTcp_HCLIENT hClient);

//等待 線程 停止 工作
KING_C_API
void KING_C_CALL KTcp_JoinClient(KTcp_HCLIENT hClient);

//向 客戶端 發送 隊列 寫入一條 發送 數據
KING_C_API
BOOL KING_C_CALL KTcp_ClientWriteBytes(KTcp_HCLIENT hClient,KTcp_HSOCKET hSocket,const king::byte_t* bytes,std::size_t n);

KING_C_API
BOOL KING_C_CALL KTcp_ClientWriteBuffer(KTcp_HCLIENT hClient,KTcp_HSOCKET hSocket,KTcp_HBUFFER hBuffer);

//註冊 斷開 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnClose(KTcp_HCLIENT hClient,KTcp_FuncClientOnClose func);

//註冊 recv 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnRecv(KTcp_HCLIENT hClient,KTcp_FuncClientOnRecv func);

//註冊 send 回調
KING_C_API
void KING_C_CALL KTcp_SetClientOnSend(KTcp_HCLIENT hClient,KTcp_FuncClientOnSend func);

#endif // CLIENT_H_INCLUDED
