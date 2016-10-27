/*  iconv 庫提供的 api 雖然簡單 然在c++中使用 顯得 有些麻煩
    這是將 iconv 包裝的 一個 class 以便於 使用


*/
#ifndef KING_LIB_HEADER_STRINGS_ICONV
#define KING_LIB_HEADER_STRINGS_ICONV

#include <iconv.h>
#include <memory>

#include <king/exception.hpp>

namespace king
{
namespace strings
{

    //創建轉碼設備失敗
    class BadIconvOpen
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::strings::BadIconvOpen";
        }
    };
    //申請內存 存儲 iconv 設備失敗
    class BadIconvAlloc
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::strings::BadIconvAlloc";
        }
    };

    //轉碼失敗
    class BadIconvTranslate
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::strings::BadIconvTranslate";
        }
    };
    class BadIconvTranslateAlloc
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::strings::BadIconvTranslateAlloc";
        }
    };
#define KING_STRINGS_CODE_UTF8      "UTF-8"
#define KING_STRINGS_CODE_UTF16     "UTF-16"
#define KING_STRINGS_CODE_UTF8BE    "UTF-16BE"
#define KING_STRINGS_CODE_UTF8LE    "UTF-16LE"

#define KING_STRINGS_CODE_GBK   "GBK"

#define KING_STRINGS_ICONV_NONE ((::iconv_t)-1)
#define KING_STRINGS_ICONV_ERROR ((::size_t)-1)

    class Iconv
    {
    protected:
        typedef iconv_t T;
        typedef std::shared_ptr<T> _icvSpt;

        _icvSpt _icv;
        static void D(T* p)
        {
            iconv_close(*p);
            delete p;
        }
    public:
        explicit Iconv(const char* tocode = KING_STRINGS_CODE_GBK, const char* fromcode = KING_STRINGS_CODE_UTF8)//throw BadIconvOpen BadIconvAlloc
        {
            auto icv = iconv_open(tocode,fromcode);
            if(icv == KING_STRINGS_ICONV_NONE)
            {
                throw BadIconvOpen();
            }

            auto p = new(std::nothrow) T(icv);
            if(!p)
            {
                throw BadIconvAlloc();
            }

            _icv = std::shared_ptr<T>(p,Iconv::D);
        }
        Iconv(const Iconv& copy)
        {
            _icv = copy._icv;
        }
        Iconv& operator=(const Iconv& copy)
        {
            _icv = copy._icv;
            return *this;
        }
        Iconv(Iconv&& copy)
        {
            _icv = copy._icv;
            copy._icv.reset();
        }
        Iconv& operator=(Iconv&& copy)
        {
            _icv = copy._icv;
            copy._icv.reset();
            return *this;
        }

    public:
        //轉碼 並返回 轉碼後 文本 長度
        std::size_t Translate(const char* in,std::size_t isize,char* out,std::size_t osize) //throw BadIconvTranslate
        {
            char* p_out = out;
            std::size_t o_size = osize;
            if(KING_STRINGS_ICONV_ERROR == ::iconv(*_icv,&in,&isize,&p_out,&o_size))
            {
                throw BadIconvTranslate();
            }
            return osize - o_size;
        }
        std::size_t Translate(const std::string& in,char* out,std::size_t osize) //throw BadIconvTranslate
        {
            return Translate(in.data(),in.size(),out,osize);
        }
        inline bool TryTranslate(const char* in,std::size_t isize,char* out,std::size_t& osize)
        {
            try
            {
                osize = Translate(in,isize,out,osize);
                return true;
            }
            catch(...)
            {

            }
            return false;
        }
        inline bool TryTranslate(const std::string& in,char* out,std::size_t& osize)
        {
            try
            {
                osize = Translate(in.data(),in.size(),out,osize);
                return true;
            }
            catch(...)
            {

            }
            return false;
        }


    public:
        //轉碼
        //osize 如不設置 為0 將 以 in.size() * 4 建立轉碼緩衝區
        //如果  osize 指示 的緩衝區 不夠 將 throw BadIconvTranslateAlloc
        std::string Translate(const char* in,std::size_t isize,std::size_t osize = 0)//throw BadIconvTranslate BadIconvTranslateAlloc
        {
            try
            {
                if(!osize)
                {
                    osize = isize * 4;
                }
                char* out = new char[osize];

                try
                {
                    std::size_t n = Translate(in,isize,out,osize);
                    std::string str(out,n);
                    delete out;

                    return std::move(str);
                }
                catch(...)
                {
                    delete out;
                    throw;
                }
            }
            catch(const std::bad_alloc&)
            {
                throw BadIconvTranslateAlloc();
            }
            return std::string();
        }

        inline std::string Translate(const std::string& in,std::size_t osize = 0)
        {
            return Translate(in.data(),in.size(),osize);
        }
        //轉碼
        //失敗 不拋出異常 而是 返回 false
        inline bool TryTranslate(const std::string& in,std::string& out,std::size_t osize = 0)
        {
            try
            {
                out = Translate(in.data(),in.size(),osize);
                return true;
            }
            catch(...)
            {
            }
            return false;
        }
        inline bool TryTranslate(const char* in,std::size_t isize,std::string& out,std::size_t osize = 0)
        {
            try
            {
                out = Translate(in,isize,osize);
                return true;
            }
            catch(...)
            {
            }
            return false;
        }


    };



};
};

#endif // KING_LIB_HEADER_STRINGS_ICONV
