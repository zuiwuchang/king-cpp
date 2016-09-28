#ifndef C_DYNAMIC_KTCP_HPP_INCLUDED
#define C_DYNAMIC_KTCP_HPP_INCLUDED

#include <windows.h>

#define KING_C_CALL  __cdecl

#ifndef BOOL
#define BOOL int
#endif // BOOL

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // FALSE



#define KTCP_NET_TCP_ERROR_MSG          ((std::size_t)-1)



#define KTCP_VAR_FUNC(Name) Name##_F Name;
#define KTCP_RESET_FUNC(Name) Name=NULL;

#define KTCP_GET_FUNC2(Name,Func) Name=(Name##_F)GetProcAddress(_h,#Func);\
	if(!Name){\
	reset();\
	return false;}
#define KTCP_GET_FUNC(Name) KTCP_GET_FUNC2(Name,KTcp_##Name)


namespace ktcp
{
    //定義 字節 型別
    typedef BYTE byte_t;



    //緩衝區 定義
    typedef struct _BUFFER_
    {
    }BUFFER,*HBUFFER;

    //創建 緩衝區
    typedef HBUFFER (KING_C_CALL *CreateBuffer_F)(const std::size_t size);

    //創建 buffer 的一個副本 失敗 返回 NULL
    //需要手動使用 DestoryBuffer 銷毀副本
    typedef HBUFFER (KING_C_CALL *CopyBuffer_F)(HBUFFER hBuffer);

    //銷毀 緩衝區
    typedef void (KING_C_CALL *DestoryBuffer_F)(HBUFFER hBuffer);

    //返回 緩衝區 指針
    typedef byte_t* (KING_C_CALL *GetBufferPtr_F)(HBUFFER hBuffer);

    //返回 緩衝區 大小
    typedef std::size_t (KING_C_CALL *GetBufferSize_F)(HBUFFER hBuffer);



    //解析器 定義
    typedef struct _READER_
    {
    }READER,*HREADER;

    //協議解析 回調函數
    //傳入 消息頭 返回 消息長度
    //返回 KTCP_NET_TCP_ERROR_MSG 代表 協議錯誤 將 對開對應 tcp
    typedef std::size_t (KING_C_CALL *FuncGetMessageSize_F)(const byte_t* b,std::size_t n);

    //創建 消息 解析器
    typedef HREADER (KING_C_CALL *CreateReader_F)(const std::size_t hsize,FuncGetMessageSize_F);

    //銷毀 消息 解析器
    typedef void (KING_C_CALL *DestoryReader_F)(HREADER hReader);



    //socket 定義
    typedef struct _SOCKET_
    {
    }SOCKET,*HSOCKET;

    //創建 socket 的一個副本 失敗 返回 NULL
    //需要手動使用 DestorySocket 銷毀副本
    typedef HSOCKET (KING_C_CALL *CopySocket_F)(HSOCKET hSocket);

    //銷毀 socket
    typedef void (KING_C_CALL *DestorySocket_F)(HSOCKET hSocket);

    //斷開 socket 連接
    typedef void (KING_C_CALL *CloseSocket_F)(HSOCKET hSocket);

    //設置 socket 關聯數據
    typedef void (KING_C_CALL *SetSocketData_F)(HSOCKET hSocket,const std::size_t data);

    //獲取 socket 關聯數據
    typedef std::size_t (KING_C_CALL *GetSocketData_F)(HSOCKET hSocket);

    //獲取 socket 本地 句柄
    typedef std::size_t (KING_C_CALL *GetSocketNative_F)(HSOCKET hSocket);

    //返回 本地 端口
    typedef unsigned short (KING_C_CALL *GetSocketLocalPort_F)(HSOCKET hSocket);

    //返回 本地 字符串長度 不包含 0
    //如果 buf 足夠 將 ip copy到 buf中
    typedef std::size_t (KING_C_CALL *GetSocketLocalIp_F)(HSOCKET hSocket,char* buf,std::size_t n);

    //返回 遠端 端口
    typedef unsigned short (KING_C_CALL *GetSocketRemotePort_F)(HSOCKET hSocket);

    //返回 遠端 字符串長度 不包含 0
    //如果 buf 足夠 將 ip copy到 buf中
    typedef std::size_t (KING_C_CALL *GetSocketRemoteIp_F)(HSOCKET hSocket,char* buf,std::size_t n);



    //服務器 定義
    typedef struct _SERVER_
    {
    }SERVER,*HSERVER;

    //回調 函數 定義
    typedef void (KING_C_CALL *FuncServerOnAccept_F)(HSERVER,HSOCKET);
    typedef void (KING_C_CALL *FuncServerOnClose_F)(HSERVER,HSOCKET);
    typedef void (KING_C_CALL *FuncServerOnRecv_F)(HSERVER,HSOCKET,HBUFFER);
    typedef void (KING_C_CALL *FuncServerOnSend_F)(HSERVER,HSOCKET,HBUFFER);

    //創建 服務器
    typedef HSERVER (KING_C_CALL *CreateServer_F)(const unsigned short port,HREADER hReader);

    //銷毀 服務器
    typedef void (KING_C_CALL *DestoryServer_F)(HSERVER hServer);


    //啟動 工作 線程
    typedef void (KING_C_CALL *RunServer_F)(HSERVER hServer);

    //停止 服務
    typedef void (KING_C_CALL *StopServer_F)(HSERVER hServer);

    //返回 服務是否 停止
    typedef BOOL (KING_C_CALL *IsServerStopped_F)(HSERVER hServer);

    //返回 工作 線程 數量
    typedef std::size_t (KING_C_CALL *GetServerThreads_F)(HSERVER hServer);

    //等待 線程 停止 工作
    typedef void (KING_C_CALL *JoinServer_F)(HSERVER hServer);

    //向 客戶端 發送 隊列 寫入一條 發送 數據
    typedef BOOL (KING_C_CALL *ServerWriteBytes_F)(HSERVER hServer,HSOCKET hSocket,const byte_t* bytes,std::size_t n);
    typedef BOOL (KING_C_CALL *ServerWriteBuffer_F)(HSERVER hServer,HSOCKET hSocket,HBUFFER hBuffer);

    //註冊 連接 回調
    typedef void (KING_C_CALL *SetServerOnAccept_F)(HSERVER hServer,FuncServerOnAccept_F func);

    //註冊 斷開 回調
    typedef void (KING_C_CALL *SetServerOnClose_F)(HSERVER hServer,FuncServerOnClose_F func);

    //註冊 recv 回調
    typedef void (KING_C_CALL *SetServerOnRecv_F)(HSERVER hServer,FuncServerOnRecv_F func);

    //註冊 send 回調
    typedef void (KING_C_CALL *SetServerOnSend_F)(HSERVER hServer,FuncServerOnSend_F func);



    //客戶端 定義
    typedef struct _CLIENT_
    {
    }CLIENT,*HCLIENT;

    //回調 函數 定義
    typedef void (KING_C_CALL *FuncClientOnClose_F)(HCLIENT,HSOCKET);
    typedef void (KING_C_CALL *FuncClientOnRecv_F)(HCLIENT,HSOCKET,HBUFFER);
    typedef void (KING_C_CALL *FuncClientOnSend_F)(HCLIENT,HSOCKET,HBUFFER);

    //創建 客戶端
    typedef HCLIENT (KING_C_CALL *CreateClient_F)(const char* addr,const unsigned short port,HREADER hReader);

    //銷毀 客戶端
    typedef void (KING_C_CALL *DestoryClient_F)(HCLIENT hClient);

    //啟動 工作 線程
    typedef void (KING_C_CALL *RunClient_F)(HCLIENT hClient);

    //停止 服務
    typedef void (KING_C_CALL *StopClient_F)(HCLIENT hClient);

    //返回 服務是否 停止
    typedef BOOL (KING_C_CALL *IsClientStopped_F)(HCLIENT hClient);

    //返回 工作 線程 數量
    typedef std::size_t (KING_C_CALL *GetClientThreads_F)(HCLIENT hClient);

    //等待 線程 停止 工作
    typedef void (KING_C_CALL *JoinClient_F)(HCLIENT hClient);

    //向 服務器 發送 隊列 寫入一條 發送 數據
    typedef BOOL (KING_C_CALL *ClientWriteBytes_F)(HCLIENT hClient,HSOCKET hSocket,const byte_t* bytes,std::size_t n);
    typedef BOOL (KING_C_CALL *ClientWriteBuffer_F)(HCLIENT hClient,HSOCKET hSocket,HBUFFER hBuffer);

    //註冊 斷開 回調
    typedef void (KING_C_CALL *SetClientOnClose_F)(HCLIENT hClient,FuncClientOnClose_F func);

    //註冊 recv 回調
    typedef void (KING_C_CALL *SetClientOnRecv_F)(HCLIENT hClient,FuncClientOnRecv_F func);

    //註冊 send 回調
    typedef void (KING_C_CALL *SetClientOnSend_F)(HCLIENT hClient,FuncClientOnSend_F func);



    //模塊 定義
    class funcs_t
    {
    private:
        funcs_t(const funcs_t& copy);
        funcs_t& operator=(const funcs_t& copy);
        funcs_t()
        {
            _h = NULL;
            reset_funcs();
        }
        HMODULE _h;
    public:
        ~funcs_t()
        {

        }
        //返回 單件 實例
        static funcs_t& instance()
        {
            static funcs_t s;
            return s;
        }

        //返回 單件 是否 初始化 成功
        inline bool ok()const
        {
            return _h != NULL;
        }
        inline operator bool()const
        {
            return _h != NULL;
        }

        //加載 dll
        bool load_library(const std::wstring& dll)
        {
            if(_h)
            {
                return true;
            }

            _h = LoadLibraryW(dll.c_str());
            if(_h == NULL)
            {
                return false;
            }

            return get_funcs();
        }
        //加載 dll
        bool load_library(const std::string& dll)
        {
            if(_h)
            {
                return true;
            }

            _h = LoadLibraryA(dll.c_str());
            if(_h == NULL)
            {
                return false;
            }

            return get_funcs();
        }
        //釋放 dll
        void reset()
        {
            if(!_h)
            {
                return;
            }
            FreeLibrary(_h);
            _h = NULL;

            reset_funcs();
        }
    public:
        KTCP_VAR_FUNC(CreateBuffer)
        KTCP_VAR_FUNC(CopyBuffer)
        KTCP_VAR_FUNC(DestoryBuffer)
        KTCP_VAR_FUNC(GetBufferPtr)
        KTCP_VAR_FUNC(GetBufferSize)

        KTCP_VAR_FUNC(CreateReader)
        KTCP_VAR_FUNC(DestoryReader)

        KTCP_VAR_FUNC(CopySocket)
        KTCP_VAR_FUNC(DestorySocket)
        KTCP_VAR_FUNC(CloseSocket)
        KTCP_VAR_FUNC(SetSocketData)
        KTCP_VAR_FUNC(GetSocketData)
        KTCP_VAR_FUNC(GetSocketNative)
        KTCP_VAR_FUNC(GetSocketLocalPort)
        KTCP_VAR_FUNC(GetSocketLocalIp)
        KTCP_VAR_FUNC(GetSocketRemotePort)
        KTCP_VAR_FUNC(GetSocketRemoteIp)

        KTCP_VAR_FUNC(CreateServer)
        KTCP_VAR_FUNC(DestoryServer)
        KTCP_VAR_FUNC(RunServer)
        KTCP_VAR_FUNC(StopServer)
        KTCP_VAR_FUNC(IsServerStopped)
        KTCP_VAR_FUNC(GetServerThreads)
        KTCP_VAR_FUNC(JoinServer)
        KTCP_VAR_FUNC(ServerWriteBytes)
        KTCP_VAR_FUNC(ServerWriteBuffer)
        KTCP_VAR_FUNC(SetServerOnAccept)
        KTCP_VAR_FUNC(SetServerOnClose)
        KTCP_VAR_FUNC(SetServerOnRecv)
        KTCP_VAR_FUNC(SetServerOnSend)

        KTCP_VAR_FUNC(CreateClient)
        KTCP_VAR_FUNC(DestoryClient)
        KTCP_VAR_FUNC(RunClient)
        KTCP_VAR_FUNC(StopClient)
        KTCP_VAR_FUNC(IsClientStopped)
        KTCP_VAR_FUNC(GetClientThreads)
        KTCP_VAR_FUNC(JoinClient)
        KTCP_VAR_FUNC(ClientWriteBytes)
        KTCP_VAR_FUNC(ClientWriteBuffer)
        KTCP_VAR_FUNC(SetClientOnClose)
        KTCP_VAR_FUNC(SetClientOnRecv)
        KTCP_VAR_FUNC(SetClientOnSend)
    private:
        void reset_funcs()
        {
            KTCP_RESET_FUNC(CreateBuffer)
            KTCP_RESET_FUNC(CopyBuffer)
            KTCP_RESET_FUNC(DestoryBuffer)
            KTCP_RESET_FUNC(GetBufferPtr)
            KTCP_RESET_FUNC(GetBufferSize)

            KTCP_RESET_FUNC(CreateReader)
            KTCP_RESET_FUNC(DestoryReader)

            KTCP_RESET_FUNC(CopySocket)
            KTCP_RESET_FUNC(DestorySocket)
            KTCP_RESET_FUNC(CloseSocket)
            KTCP_RESET_FUNC(SetSocketData)
            KTCP_RESET_FUNC(GetSocketData)
            KTCP_RESET_FUNC(GetSocketNative)
            KTCP_RESET_FUNC(GetSocketLocalPort)
            KTCP_RESET_FUNC(GetSocketLocalIp)
            KTCP_RESET_FUNC(GetSocketRemotePort)
            KTCP_RESET_FUNC(GetSocketRemoteIp)

            KTCP_RESET_FUNC(CreateServer)
            KTCP_RESET_FUNC(DestoryServer)
            KTCP_RESET_FUNC(RunServer)
            KTCP_RESET_FUNC(StopServer)
            KTCP_RESET_FUNC(IsServerStopped)
            KTCP_RESET_FUNC(GetServerThreads)
            KTCP_RESET_FUNC(JoinServer)
            KTCP_RESET_FUNC(ServerWriteBytes)
            KTCP_RESET_FUNC(ServerWriteBuffer)
            KTCP_RESET_FUNC(SetServerOnAccept)
            KTCP_RESET_FUNC(SetServerOnClose)
            KTCP_RESET_FUNC(SetServerOnRecv)
            KTCP_RESET_FUNC(SetServerOnSend)

            KTCP_RESET_FUNC(CreateClient)
            KTCP_RESET_FUNC(DestoryClient)
            KTCP_RESET_FUNC(RunClient)
            KTCP_RESET_FUNC(StopClient)
            KTCP_RESET_FUNC(IsClientStopped)
            KTCP_RESET_FUNC(GetClientThreads)
            KTCP_RESET_FUNC(JoinClient)
            KTCP_RESET_FUNC(ClientWriteBytes)
            KTCP_RESET_FUNC(ClientWriteBuffer)
            KTCP_RESET_FUNC(SetClientOnClose)
            KTCP_RESET_FUNC(SetClientOnRecv)
            KTCP_RESET_FUNC(SetClientOnSend)

        }
        bool get_funcs()
        {
            KTCP_GET_FUNC(CreateBuffer)
            KTCP_GET_FUNC(CopyBuffer)
            KTCP_GET_FUNC(DestoryBuffer)
            KTCP_GET_FUNC(GetBufferPtr)
            KTCP_GET_FUNC(GetBufferSize)

            KTCP_GET_FUNC(CreateReader)
            KTCP_GET_FUNC(DestoryReader)

            KTCP_GET_FUNC(CopySocket)
            KTCP_GET_FUNC(DestorySocket)
            KTCP_GET_FUNC(CloseSocket)
            KTCP_GET_FUNC(SetSocketData)
            KTCP_GET_FUNC(GetSocketData)
            KTCP_GET_FUNC(GetSocketNative)
            KTCP_GET_FUNC(GetSocketLocalPort)
            KTCP_GET_FUNC(GetSocketLocalIp)
            KTCP_GET_FUNC(GetSocketRemotePort)
            KTCP_GET_FUNC(GetSocketRemoteIp)

            KTCP_GET_FUNC(CreateServer)
            KTCP_GET_FUNC(DestoryServer)
            KTCP_GET_FUNC(RunServer)
            KTCP_GET_FUNC(StopServer)
            KTCP_GET_FUNC(IsServerStopped)
            KTCP_GET_FUNC(GetServerThreads)
            KTCP_GET_FUNC(JoinServer)
            KTCP_GET_FUNC(ServerWriteBytes)
            KTCP_GET_FUNC(ServerWriteBuffer)
            KTCP_GET_FUNC(SetServerOnAccept)
            KTCP_GET_FUNC(SetServerOnClose)
            KTCP_GET_FUNC(SetServerOnRecv)
            KTCP_GET_FUNC(SetServerOnSend)

            KTCP_GET_FUNC(CreateClient)
            KTCP_GET_FUNC(DestoryClient)
            KTCP_GET_FUNC(RunClient)
            KTCP_GET_FUNC(StopClient)
            KTCP_GET_FUNC(IsClientStopped)
            KTCP_GET_FUNC(GetClientThreads)
            KTCP_GET_FUNC(JoinClient)
            KTCP_GET_FUNC(ClientWriteBytes)
            KTCP_GET_FUNC(ClientWriteBuffer)
            KTCP_GET_FUNC(SetClientOnClose)
            KTCP_GET_FUNC(SetClientOnRecv)
            KTCP_GET_FUNC(SetClientOnSend)

            return true;
        }
    };

};

#endif // C_DYNAMIC_KTCP_HPP_INCLUDED
