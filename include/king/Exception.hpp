//異常定義
#ifndef KING_LIB_HEADER_EXCEPTION
#define KING_LIB_HEADER_EXCEPTION

#include <string>

namespace king
{
    /**
    *   \brief 異常基類
    */
    class Exception
    {
    public:
        explicit Exception(){}
        virtual ~Exception(){}
        /**
        *   \brief 返回 異常 描述字符串
        */
        virtual const char* What() const
        {
            return "king::Exception";
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
