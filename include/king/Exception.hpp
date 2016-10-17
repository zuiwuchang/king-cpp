//異常定義
#ifndef KING_LIB_HEADER_EXCEPTION
#define KING_LIB_HEADER_EXCEPTION

#include <string>

namespace king
{
    class Exception
    {
    protected:
        std::string _what;
        std::size_t _code = 0;
        bool _e = false;
    public:
        explicit Exception()
        {
        }
        explicit Exception(const std::string& what,const std::size_t code = 0)
            :_what(what),
            _code(code),
            _e(true)
        {
        }
        explicit Exception(const std::size_t code)
            :_code(code),
            _e(true)
        {
        }
        inline const std::string& GetWhat()const
        {
            return _what;
        }
        inline const std::size_t GetCode()const
        {
            return _code;
        }

        explicit inline operator bool()
        {
            return _e;
        }
    };


    //為兼容舊風格 的保留代碼
    class exception
    {
    protected:
        std::string _what;
        std::size_t _code = 0;
        bool _e = false;
    public:
        explicit exception()
        {
        }
        explicit exception(const std::string& what,const std::size_t code = 0)
            :_what(what),
            _code(code),
            _e(true)
        {
        }
        explicit exception(const std::size_t code)
            :_code(code),
            _e(true)
        {
        }
        inline const std::string& what()const
        {
            return _what;
        }
        inline const std::size_t code()const
        {
            return _code;
        }

        explicit inline operator bool()
        {
            return _e;
        }
    };
};

#endif // KING_LIB_HEADER_EXCEPTION
