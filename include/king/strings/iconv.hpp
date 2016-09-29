//iconv 庫提供的 api 雖然簡單 然在c++中使用 顯得 有些麻煩
//這是將 iconv 包裝的 一個 class 以便於 使用
#ifndef KING_LIB_HEADER_STRINGS_ICONV
#define KING_LIB_HEADER_STRINGS_ICONV

#include <iconv.h>
#include <memory>

#include <king/exception.hpp>

namespace king
{
namespace strings
{
#define KING_STRINGS_CODE_UTF8      "UTF-8"
#define KING_STRINGS_CODE_UTF16     "UTF-16"
#define KING_STRINGS_CODE_UTF8BE    "UTF-16BE"
#define KING_STRINGS_CODE_UTF8LE    "UTF-16LE"

#define KING_STRINGS_CODE_GBK   "GBK"

#define KING_STRINGS_ICONV_NONE ((::iconv_t)-1)
#define KING_STRINGS_ICONV_ERROR ((::size_t)-1)
    class bad_iconv_alloc
        :  public king::exception
    {
    public:
        using king::exception::exception;

    };

    class bad_iconv_code
        :  public king::exception
    {
    public:
        using king::exception::exception;
    };
    class bad_iconv_code_alloc
        :  public king::exception
    {
    public:
        using king::exception::exception;
    };

    class iconv_t
    {
    private:
        typedef ::iconv_t T;
        typedef std::shared_ptr<T> icv_spt;
    protected:
        icv_spt _icv;
        static void D(T* p)
        {
            iconv_close(*p);
            delete p;
        }
    public:
        explicit iconv_t(const char* tocode = KING_STRINGS_CODE_GBK, const char* fromcode = KING_STRINGS_CODE_UTF8)//throw bad_iconv_alloc
        {
            auto icv = iconv_open(tocode,fromcode);
            if(icv == KING_STRINGS_ICONV_NONE)
            {
                throw bad_iconv_alloc("iconv_open error");
            }


            auto p = new(std::nothrow) T(icv);
            if(!p)
            {
                throw bad_iconv_alloc("alloc iconv_t error");
            }

            _icv = std::shared_ptr<T>(p,iconv_t::D);
        }
        iconv_t(const iconv_t& copy)
        {
            _icv = copy._icv;
        }
        iconv_t& operator=(const iconv_t& copy)
        {
            _icv = copy._icv;
            return *this;
        }
        iconv_t(iconv_t&& copy)
        {
            _icv = copy._icv;
            copy._icv.reset();
        }
        iconv_t& operator=(iconv_t&& copy)
        {
            _icv = copy._icv;
            copy._icv.reset();
            return *this;
        }
        explicit inline operator bool()
        {
            return _icv?true:false;
        }
        inline bool empty()
        {
            return _icv?false:true;
        }

        //轉碼 返回 轉碼後 文本 長度
        std::size_t iconv(const std::string& in,char* out,std::size_t osize)//throw bad_iconv_code
        {
            const char* p_in = in.data();
            std::size_t i_size = in.size();

            char* p_out = out;
            std::size_t o_size = osize;
            if(KING_STRINGS_ICONV_ERROR == ::iconv(*_icv,&p_in,&i_size,&p_out,&o_size))
            {
                throw bad_iconv_code("iconv error");
            }

            return osize - o_size;
        }
        //osize 如不設置 為0 將 以 in.size() * 4 建立轉碼緩衝區
        //如果  osize 只示 的緩衝區 不夠 將 throw std::bad_alloc
        std::string iconv(const std::string& in,std::size_t osize = 0)//throw bad_iconv_code bad_iconv_alloc
        {
            try
            {
                if(!osize)
                {
                    osize = in.size() * 4;
                }
                char* out = new char[osize];

                try
                {
                    std::size_t n = iconv(in,out,osize);
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
                throw bad_iconv_alloc("bad alloc");
            }
            return std::string();
        }

        //轉碼 返回 轉碼後 文本 長度
        std::size_t iconv(const char* in,std::size_t isize,char* out,std::size_t osize)//throw bad_iconv_code
        {
            char* p_out = out;
            std::size_t o_size = osize;
            if(KING_STRINGS_ICONV_ERROR == ::iconv(*_icv,&in,&isize,&p_out,&o_size))
            {
                throw bad_iconv_code("iconv error");
            }

            return osize - o_size;
        }

         //轉碼 返回 轉碼後 文本 長度
        std::string iconv(const char* in,std::size_t isize,std::size_t osize = 0)//throw bad_iconv_code bad_iconv_alloc
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
                    std::size_t n = iconv(in,isize,out,osize);
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
                throw bad_iconv_alloc("bad alloc");
            }
            return std::string();
        }

    };


};
};

#endif // KING_LIB_HEADER_STRINGS_ICONV
