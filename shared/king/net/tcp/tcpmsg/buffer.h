#ifndef BUFFER_H_INCLUDED
#define BUFFER_H_INCLUDED

#include <king/bytes/type.hpp>

#include "configure.h"


typedef std::shared_ptr<king::bytes::bytes_t> KTcp_BUFFER;
typedef KTcp_BUFFER* KTcp_HBUFFER;

//創建 緩衝區
KING_C_API
KTcp_HBUFFER KING_C_CALL KTcp_CreateBuffer(const std::size_t size);

//創建 buffer 的一個副本 失敗 返回 NULL
//需要手動使用 DestoryBuffer 銷毀副本
KING_C_API
KTcp_HBUFFER KING_C_CALL KTcp_CopyBuffer(KTcp_HBUFFER hBuffer);

//銷毀 緩衝區
KING_C_API
void KING_C_CALL KTcp_DestoryBuffer(KTcp_HBUFFER hBuffer);


//返回 緩衝區 指針
KING_C_API
king::byte_t* KING_C_CALL KTcp_GetBufferPtr(KTcp_HBUFFER hBuffer);

//返回 緩衝區 大小
KING_C_API
std::size_t KING_C_CALL KTcp_GetBufferSize(KTcp_HBUFFER hBuffer);
#endif // BUFFER_H_INCLUDED
