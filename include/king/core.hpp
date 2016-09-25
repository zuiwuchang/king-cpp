//基本數據 型別定義
#ifndef KING_LIB_HEADER_CORE
#define KING_LIB_HEADER_CORE

#include <cstdint>
#include <memory>

namespace king
{
    /*******    int    *******/
    typedef std::int8_t int8_t;
    typedef std::int16_t int16_t;
    typedef std::int32_t int32_t;

    typedef std::uint8_t uint8_t;
    typedef std::uint16_t uint16_t;
    typedef std::uint32_t uint32_t;

    typedef std::uint8_t byte_t;


    /*******    factory    *******/
    template<typename T>
    class c_pointer_factory
    {
    public:
        //創建 n 個 T 的指針
        static T* create(const std::size_t n)
        {
            return (T*)malloc(sizeof(T) * n);
        }
        //釋放 指針
        static destory(T* p)
        {
            free(p);
        }
    };
};

#endif // KING_LIB_HEADER_CORE
