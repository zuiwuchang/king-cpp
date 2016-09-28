#include "buffer.h"

//創建 緩衝區
KING_C_API
KTcp_HBUFFER KING_C_CALL KTcp_CreateBuffer(const std::size_t size)
{
    KTcp_HBUFFER hBuffer = NULL;
    try
    {
        hBuffer = new KTcp_BUFFER(std::make_shared<king::bytes::bytes_t>(size));
        if(hBuffer->get()->empty())
        {
            delete hBuffer;
            hBuffer = NULL;
        }
    }
    catch(const std::bad_alloc&)
    {

    }
    return hBuffer;
}

//創建 buffer 的一個副本 失敗 返回 NULL
//需要手動使用 DestoryBuffer 銷毀副本
KING_C_API
KTcp_HBUFFER KING_C_CALL KTcp_CopyBuffer(KTcp_HBUFFER hBuffer)
{
    KTcp_HBUFFER p = NULL;
    try
    {
        p = new KTcp_BUFFER();
        *p = *hBuffer;
    }
    catch(const std::bad_alloc&)
    {

    }
    return p;
}

//銷毀 緩衝區
KING_C_API
void KING_C_CALL KTcp_DestoryBuffer(KTcp_HBUFFER hBuffer)
{
    delete hBuffer;
}

//返回 緩衝區 指針
KING_C_API
king::byte_t* KING_C_CALL KTcp_GetBufferPtr(KTcp_HBUFFER hBuffer)
{
    return (*hBuffer)->get();
}

//返回 緩衝區 大小
KING_C_API
std::size_t KING_C_CALL KTcp_GetBufferSize(KTcp_HBUFFER hBuffer)
{
    return (*hBuffer)->size();
}
