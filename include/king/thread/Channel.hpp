/*  golang 中的 channel 實在讓人 垂涎欲滴
    這是 孤在 c++ 中實現的 一個 類似的 channel

    用法基本同 golang channel
    只是現在 使用 Channel<T,N> 創建 channel
    使用 << 寫入數據到 channel
    使用 >> 從channel 讀取數據

    要求 :
        channel 傳輸的 數據 必須滿足 copy 語義
        channel 傳輸的 數據 必須存在 無參 T() 構造函數

    用法 :
        基本 和 golang channel 一樣
        只是 使用 << >> 讀寫數據

    依賴 :
        boost::mutex
            (boost lib : system)

*/
#ifndef KING_LIB_HEADER_THREAD_CHANNEL
#define KING_LIB_HEADER_THREAD_CHANNEL


#include <boost/thread/mutex.hpp>

#include <memory>

#include <king/container/LoopBuffer.hpp>
namespace king
{
    namespace thread
    {
        class ClosedChannelWrite
            :public king::Exception
        {
        public:
            virtual const char* What()
            {
                return "king::thread::ClosedChannelWrite";

            }
        };
        class ClosedChannelRead
            :public king::Exception
        {
        public:
            virtual const char* What()
            {
                return "king::thread::ClosedChannelRead";
            }
        };

        //T channel 傳輸的數據
        //N 緩存大小
        template<typename T,std::size_t N = 0>
        class Channel
        {
        protected:
            //同步mutex
            std::shared_ptr<boost::mutex> _mutexWrite;
            std::shared_ptr<boost::mutex> _mutexRead;
            std::shared_ptr<boost::mutex> _mutex;

            //數據 緩衝區
            typedef king::container::LoopBuffer<T,N+1> LoopBuffer;
            std::shared_ptr<LoopBuffer> _buffer;

            //是否 已經 關閉 操作流
            bool _closeWrite = false;
            bool _closeRead = false;
        public:
           explicit Channel()
           {
               _mutex = std::make_shared<boost::mutex>();
               _mutexWrite = std::make_shared<boost::mutex>();
               _mutexRead = std::make_shared<boost::mutex>();
               _mutexRead->lock();
               _buffer = std::make_shared<LoopBuffer>();
           }
           Channel(const Channel& copy)
           {
               _mutex = copy._mutex;
               _mutexWrite = copy._mutexWrite;
               _mutexRead = copy._mutexRead;
               _buffer = copy._buffer;

               _closeWrite = copy._closeWrite;
               _closeRead = copy._closeRead;
           }
           Channel& operator=(const Channel& copy)
           {
               _mutex = copy._mutex;
               _mutexWrite = copy._mutexWrite;
               _mutexRead = copy._mutexRead;
               _buffer = copy._buffer;

               _closeWrite = copy._closeWrite;
               _closeRead = copy._closeRead;
               return *this;
           }

           //寫入數據 到 channel
           bool Write(const T& v)
           {
               while(true)
               {
                   try
                   {
                       _mutexWrite->lock();

                       boost::mutex::scoped_lock lock(*_mutex);
                       if(_closeWrite)
                       {
                           return false;
                       }

                       _buffer->Write(v);
                       if(_buffer->Free())
                       {
                           unlock(_mutexWrite);
                       }

                       unlock(_mutexRead);

                       break;
                   }
                   catch(const king::container::BadLoopBufferWrite&)
                   {
                   }
               }
               return true;
           }
           Channel& operator<<(const T& v) //如果 寫入流已關閉 throw ClosedChannelWrite
           {
               if(!Write(v))
               {
                   throw ClosedChannelWrite();
               }
               return *this;
           }

           //從 channel 讀取 數據
           bool Read(T& v)
           {
               while(true)
               {
                   try
                   {
                       _mutexRead->lock();

                       boost::mutex::scoped_lock lock(*_mutex);
                       if(_closeRead)
                       {
                           return false;
                       }

                       _buffer->Read(v);
                       if(!_buffer->Empty())
                       {
                           unlock(_mutexRead);
                       }

                       unlock(_mutexWrite);

                       break;
                   }
                   catch(const king::container::BadLoopBufferRead&)
                   {
                   }
               }

               return true;
           }
           Channel& operator>>(T& v) //如果 讀取流已關閉 throw ClosedChannelRead
           {
               if(!Read(v))
               {
                   throw ClosedChannelRead();
               }
               return *this;
           }

           //返回 操作 流 是否已經 關閉
           inline bool IsCloseWrite()const
           {
               boost::mutex::scoped_lock lock(*_mutex);
               return _closeWrite;
           }
           inline bool IsCloseRead()const
           {
               boost::mutex::scoped_lock lock(*_mutex);
               return _closeRead;
           }

           //關閉 Write 流
           //使用 channel 不能在被 寫入 數據 同時 通知 所有 還在Write的 線程返回
           //(Write 將 返回 false
           //<< 將 throw ClosedChannelWrite)
           inline void CloseWrite()
           {
               boost::mutex::scoped_lock lock(*_mutex);
               _closeWrite = false;
           }
           //關閉 Read 流
           //使用 channel 不能在被 讀取 數據 同時 通知 所有 還在Read的 線程返回
           //(Read 將 返回 false
           //>> 將 throw ClosedChannelRead)
           inline void CloseRead()
           {
               boost::mutex::scoped_lock lock(*_mutex);
               _closeRead = false;
           }
           //同時關閉 Write Read 流
           inline void Close()
           {
               CloseWrite();
               CloseRead();
           }
        protected:
            inline void unlock(std::shared_ptr<boost::mutex> mutex)
            {
                mutex->try_lock();
                mutex->unlock();
            }
        };
    };
};

#endif // KING_LIB_HEADER_THREAD_CHANNEL
