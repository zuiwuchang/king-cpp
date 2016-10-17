//創建一個 console 窗口 以便調試 使用
#ifndef KING_LIB_HEADER_WINDOWS_CONSOLE
#define KING_LIB_HEADER_WINDOWS_CONSOLE

#include <windows.h>
#include <king/debug.hpp>

namespace king
{
    class Console
    {
    private:
        Console();
        ~Console();
        Console(const Console& copy);
        Console& operator=(const Console& copy);
    public:
        //創建 控制台
			static bool Alloc()
			{
				if(!AllocConsole())
				{
					return false;
				}


				static FILE *f_out = NULL;
				static FILE *f_err = NULL;
				static FILE *f_in = NULL;

				if(f_in)
				{
					fclose(f_in);
					f_in = NULL;
				}
				if(f_out)
				{
					fclose(f_out);
					f_out = NULL;
				}
				if(f_err)
				{
					fclose(f_err);
					f_err = NULL;
				}



				f_in = freopen("conin$", "r", stdin);
				if(!f_in)
				{
					return false;
				}

				f_out = freopen("conout$", "w", stdout );
				if(!f_out)
				{
					return false;
				}

				f_err = freopen( "conout$", "w", stderr );
				if(!f_err)
				{
					return false;
				}

				return true;
			}
			static bool Free()
			{
				return FreeConsole() == TRUE;
			}
    };
};
#endif // KING_LIB_HEADER_WINDOWS_CONSOLE
