//基本數據 型別定義
#ifndef KING_LIB_HEADER_CORE
#define KING_LIB_HEADER_CORE

#include <cstdint>
#include <memory>

namespace king
{
	/**
		8bit 整型
	*/
	typedef std::int8_t Int8;
	typedef std::int16_t Int16;
	typedef std::int32_t Int32;

	typedef std::uint8_t UInt8;
	typedef std::uint16_t UInt16;
	typedef std::uint32_t UInt32;

	typedef std::uint8_t Byte;






	//為兼容舊風格 的保留代碼
	/*******    int    *******/
	typedef std::int8_t int8_t;
	typedef std::int16_t int16_t;
	typedef std::int32_t int32_t;

	typedef std::uint8_t uint8_t;
	typedef std::uint16_t uint16_t;
	typedef std::uint32_t uint32_t;

	typedef std::uint8_t byte_t;

};

#endif // KING_LIB_HEADER_CORE
