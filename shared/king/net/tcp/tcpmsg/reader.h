#ifndef READER_H_INCLUDED
#define READER_H_INCLUDED

#include <king/net/tcp/msg_reader.hpp>

#include "configure.h"
//消息 解析器 句柄
typedef king::net::tcp::msg_reader_t KTcp_READER;
typedef king::net::tcp::msg_reader_t* KTcp_HREADER;

//協議解析 函數
typedef std::size_t (KING_C_CALL *KTcp_FuncGetMessageSize)(const king::byte_t* b,std::size_t n);

//創建 消息 解析器
KING_C_API
KTcp_HREADER KING_C_CALL KTcp_CreateReader(const std::size_t hsize,KTcp_FuncGetMessageSize);

//銷毀 消息 解析器
KING_C_API
void KING_C_CALL KTcp_DestoryReader(KTcp_HREADER hReader);


#endif // READER_H_INCLUDED
