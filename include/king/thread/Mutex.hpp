/*  類似 boost::mutex 的東西
    彌補 boost::mutex 在 unlock 時 會lock 的 bug

    在 win32 下是 CRITICAL_SECTION

*/
#ifndef KING_LIB_HEADER_THREAD_MUTEX
#define KING_LIB_HEADER_THREAD_MUTEX


#ifdef WIN32
#include <windows.h>
#endif // WIN32
namespace king
{
    namespace thread
    {
        class Mutex
        {
        public:
            Mutex()
            {
                #ifdef WIN32
                InitializeCriticalSection(&_mutex);
                #endif // WIN32
            }
            ~Mutex()
            {
                #ifdef WIN32
                DeleteCriticalSection(&_mutex);
                #endif // WIN32
            }
        private:
            Mutex(const Mutex& copy) = delete;
            Mutex& operator=(const Mutex& copy) = delete;
#ifdef WIN32
            CRITICAL_SECTION _mutex = NULL;
#endif // WIN32

        public:
            inline void Lock()
            {
                #ifdef WIN32
                EnterCriticalSection(&_mutex);
                #endif // WIN32
            }
            inline void UnLock()
            {
                #ifdef WIN32
                LeaveCriticalSection(&_mutex);
                #endif // WIN32
            }
            inline void TryLock()
            {
                #ifdef WIN32
                TryEnterCriticalSection(&_mutex);
                #endif // WIN32
            }

            class ScopedLock
            {
            private:
                Mutex& _mutex;
                ScopedLock(const ScopedLock& copy) = delete;
                ScopedLock& operator=(const ScopedLock& copy) = delete;
            public:
                ScopedLock(Mutex& mutex)
                    :_mutex(mutex)
                {
                    _mutex.Lock();
                }
                ~ScopedLock()
                {
                    _mutex.UnLock();
                }
            };

            class ScopedTryLock
            {
            private:
                Mutex& _mutex;
                bool _lock;
                ScopedTryLock(const ScopedTryLock& copy) = delete;
                ScopedTryLock& operator=(const ScopedTryLock& copy) = delete;
            public:
                ScopedTryLock(Mutex& mutex)
                    :_mutex(mutex)
                {
                    _lock = _mutex.TryLock();
                }
                ~ScopedTryLock()
                {
                    if(_lock)
                    {
                        _mutex.UnLock();
                    }
                }
            };
        };

    };
};

#endif // KING_LIB_HEADER_THREAD_MUTEX
