/*  一個類似 golang channel 的東西
    用於在多線程中 不使用明確鎖的 方式傳遞數據

    //定義一個 channel 傳輸 型別為 T 緩存大小為 N
    Channel<T,N=0> ch;
    //寫入到 channel
    bool ch.Write(T);
    //從 channel 讀取
    bool ch.Read(T);
    //返回 channel 是否關閉
    bool ch.IsClose();
    //關閉 channel 以通知等待中的 Write Read 返回
    void ch.Close();

    //寫入/讀取 channel 如果失敗 立刻返回 而非阻塞
    //如果返回 false 為 open 設置 channel 是否處於打開標記
    bool TryWrite(const T& v,bool* open = nullptr)
    bool TryRead(T& v,bool* open = nullptr)

    要求 :
        傳遞數據 必須滿足 copy 語義
        傳遞數據 必須存在 無參 T() 構造函數

    依賴 :
        boost thread smart any tuple
            (boost::thread 是需要 編譯的 庫
             同時 thread 庫 依賴 boost system datatime 庫)




    golang 的 select 需要 在語言級別 提供支持
    在c++中孤只能使用 多線程 以及 宏 提供 支持
    (對於每個 要等待的 channel 都將 啟動一個 線程
     好在不會存在 太多需要同時等待的 channel)

    KING_SELECT_CHANNEL_BEGIN 和 KING_SELECT_CHANNEL_END 宏 定義一個 select的開始 和結尾 所有代碼 需要寫在兩者間

    KING_SELECT_CHANNEL_THREAD(CHAN,N) 宏 定義一個 線程 用於 等待指定的channel (CHAN)
    N 是一個 std::size_t 的任意不重複數字 用於和CHAN 進行 關聯

    KING_SELECT_CHANNEL_SWITCH 定義一個 switch 語句 將 KING_SELECT_CHANNEL_THREAD 關聯的 CHAN N 路由到 case 中

    KING_SELECT_CHANNEL_CASE(CHAN,N,VAR,OPEN,CODE) 定義一個 case 語句
    CHAN 和 N 是 KING_SELECT_CHANNEL_THREAD 中 關聯的 CHAN 和 N
    VAR 是 自定義的一個 變量名 用於 獲取 從 channel 中 讀取的 數據
    OPEN 是 自定義的一個 變量名 用於 確認通知 是因為 成功從 channel 中讀取了 數據 / 還是因為 channel 關閉 而獲取到通知(此時 VAR 中將不存在 有效數據)
    CODE 是 用戶 代碼 相當於 case N:{ 和 }break; 間的 代碼

    通常一個 select 會類似 如下代碼
    KING_SELECT_CHANNEL_BEGIN
    KING_SELECT_CHANNEL_THREAD(ch0,0)
    KING_SELECT_CHANNEL_THREAD(ch1,1)
    //... other channel
    KING_SELECT_CHANNEL_SWITCH
    KING_SELECT_CHANNEL_CASE(ch0,0,val,open,
    {
        if(open)
        {
            //do work by val
        }
        else
        {
            //do close
        }
    })
    KING_SELECT_CHANNEL_CASE(ch1,1,val,open,
    {
        if(open)
        {
            //do work by val
        }
        else
        {
            //do close
        }
    })
    //... other channel
    KING_SELECT_CHANNEL_END

    KING_SELECT_CHANNEL_BEGIN KING_SELECT_CHANNEL_END 內部 使用一個 while(true) 等待 任何 channel 的 數據通知 並且轉發到用戶代碼
    直到 所有 channel 都 關閉 才會 退出 KING_SELECT_CHANNEL_BEGIN KING_SELECT_CHANNEL_END 循環




    KING_SELECT_CHANNEL(CHAN,VAR,OK,CLOSE,CODE) 宏
    是使用 TryRead 的一個 輪詢方式的 select
    注意 此方式 只能使用在 緩存 channel 中
    TryRead 只有在 存在 數據可讀時 才會 返回 true 而對於 無緩存channel 在一個 Read 等待 Write 外的情況下 都不會寫入數據
    TryRead 不會等待 Write 所以 Write 永遠不會 有 Read 等待它 所以 Write將 不會寫入數據 造成 Read 只會返回 false

    當 使用 緩存時 即使沒有 Read 等待Write 只要 緩存 還空閒 Write 將會將數據寫入到 緩存
    而此時 TryRead 發現 緩存中存在數據 就會 返回 true 並讀取 數據

    通常一個 輪詢 select 會類似 如下代碼
    bool channel_close;
    std::size_t channel_count = ; //等待數量
    while(channel_count)
    {
        KING_SELECT_CHANNEL(ch0,val,ok,&channel_close,{
            if(ok)
            {
                //do work by val
            }
            else if(channel_close)
            {
                --channel_count;
                //do close
            }
        })
        KING_SELECT_CHANNEL(ch1,val,ok,&channel_close,{
            if(ok)
            {
                //do work by val
            }
            else if(channel_close)
            {
                --channel_count;
                //do close
            }
        })
        //... other channel

        //any wait
        //Sleep(1);
        boost::this_thread::sleep(boost::posix_time::milliseconds(1));
    }

*/


#ifndef KING_LIB_HEADER_THREAD_CHANNEL
#define KING_LIB_HEADER_THREAD_CHANNEL

#include <king/container/LoopBuffer.hpp>
#include <boost/thread/thread.hpp>
#include <boost/smart_ptr.hpp>

#include <boost/any.hpp>
#include <boost/tuple/tuple.hpp>

namespace king
{
    namespace thread
    {

        //不要直接使用 隱藏的實現代碼
        template<typename T,std::size_t N=0>
        class _channel
        {
        public:
            typedef T Data;
        protected:
            //同步mutex
            boost::mutex _mutex;

            //生產消費 通知
            boost::condition_variable _cv_producer;
            boost::condition_variable _cv_consumer;

            //數據緩存
            typedef king::container::LoopBuffer<T,N+1> LoopBuffer;
            LoopBuffer _buffer;
            //需要讀取數量
            std::size_t _need = 0;

            //是否關閉
            bool _close = false;
            boost::shared_mutex _shared_mutex;

        public:
            _channel(){}
            _channel(const _channel& copy)=delete;
            _channel& operator=(const _channel& copy)=delete;

            bool Write(const T& v)
            {
                boost::unique_lock<boost::mutex> lock(_mutex);
                while(!_buffer.Free() || (_buffer.Size() == N && !_need))
                {
                    _cv_producer.wait(lock);

                    if(IsClose())
                    {
                        _cv_consumer.notify_one();
                        return false;
                    }
                }

                if(IsClose())
                {
                    _cv_consumer.notify_one();
                    return false;
                }

                _buffer.Write(v);

                _cv_consumer.notify_one();

                return true;
            }


            //從 Channel 讀取數據
            bool Read(T& v)
            {
                boost::unique_lock<boost::mutex> lock(_mutex);
                while(_buffer.Empty())
                {
                    ++_need;
                    _cv_producer.notify_one();
                    _cv_consumer.wait(lock);
                    --_need;

                    if(IsClose())
                    {
                        _cv_producer.notify_one();
                        return false;
                    }
                }
                if(IsClose())
                {
                    _cv_producer.notify_one();
                    return false;
                }

                _buffer.Read(v);

                _cv_producer.notify_one();
                return true;
            }

            //
            inline bool IsClose()
            {
                boost::shared_lock<boost::shared_mutex> shared_lock(_shared_mutex);
                return _close;
            }
            inline void Close()
            {
                boost::unique_lock<boost::shared_mutex> shared_lock(_shared_mutex);
                _close = true;
            }

            //嘗試 寫入 數據 返回 是否 寫入 成功
            //close 返回 channel 是否 處於關閉 狀態
            //close 只有在 TryWrite 返回false 時才會被設置 在返回 true 時 不會進行任何操作
            bool TryWrite(const T& v,bool* close = nullptr)
            {
                boost::unique_lock<boost::mutex> lock(_mutex);
                if(IsClose())
                {
                    if(close)
                    {
                        *close = true;
                    }
                    _cv_consumer.notify_one();
                    return false;
                }

                if(!_buffer.Free() || (_buffer.Size() == N && !_need))
                {
                    if(close)
                    {
                        *close = false;
                    }
                    return false;
                }



                _buffer.Write(v);

                _cv_consumer.notify_one();

                return true;
            }

            //嘗試 讀出 數據 返回 是否 讀出 成功
            //...
            bool TryRead(T& v,bool* close = nullptr)
            {
                boost::unique_lock<boost::mutex> lock(_mutex);
                if(IsClose())
                {
                    if(close)
                    {
                        *close = true;
                    }
                    _cv_producer.notify_one();
                    return false;
                }
                if(_buffer.Empty())
                {
                    if(close)
                    {
                        *close = false;
                    }
                    return false;
                }

                _buffer.Read(v);

                _cv_producer.notify_one();
                return true;
            }
        };


        template<typename T,std::size_t N=0>
        class Channel
        {
        public:
            typedef T Data;
        protected:
            typedef _channel<T,N> _channel_t;
            //同步mutex
            boost::shared_ptr<_channel_t> _chan;
        public:
            Channel()
            {
                _chan = boost::make_shared<_channel_t>();
            }

            Channel(const Channel& copy)
            {
                _chan = copy._chan;
            }
            Channel& operator=(const Channel& copy)
            {
                _chan = copy._chan;
                return *this;
            }

            Channel(Channel&& copy)
            {
                _chan = copy._chan;
                copy._chan.reset();
            }
            Channel& operator=(Channel&& copy)
            {
                _chan = copy._chan;
                copy._chan.reset();
                return *this;
            }

            //寫入 數據 到 Channel
            inline bool Write(const T& v)
            {
                return _chan->Write(v);
            }

            //從 Channel 讀取數據
            inline bool Read(T& v)
            {
                return _chan->Read(v);
            }

            //
            inline bool IsClose()
            {
                return _chan->IsClose();
            }
            inline void Close()
            {
                _chan->Close();
            }

             //嘗試 寫入 數據 返回 是否 寫入 成功
            //close 返回 channel 是否 處於關閉 狀態
            //close 只有在 TryWrite 返回false 時才會被設置 在返回 true 時 不會進行任何操作
            inline bool TryWrite(const T& v,bool* close = nullptr)
            {
                return _chan->TryWrite(v,close);
            }
            //嘗試 讀出 數據 返回 是否 讀出 成功
            //...
            inline bool TryRead(T& v,bool* close = nullptr)
            {
                return _chan->TryRead(v,close);
            }
        };





#define KING_SELECT_CHANNEL_INDEX_CASE  0
#define KING_SELECT_CHANNEL_INDEX_DATA  1
#define KING_SELECT_CHANNEL_INDEX_OPEN  2
#define KING_SELECT_CHANNEL_BEGIN  {\
    typedef boost::tuples::tuple<std::size_t,boost::any,bool> _KING_any_data_t;\
    typedef king::thread::Channel<_KING_any_data_t> _KING_ch_any_t;\
    _KING_ch_any_t _KING_ch_any;\
    std::size_t _KING_sum = 0;

#define KING_SELECT_CHANNEL_THREAD(CHAN,N) \
    ++_KING_sum;\
    typedef decltype(CHAN) _KING_##CHAN##_t;\
    boost::thread _KING_thread_##CHAN([&CHAN,&_KING_ch_any]()\
    {\
        _KING_##CHAN##_t::Data data;\
        while(CHAN.Read(data))\
        {\
            _KING_ch_any.Write(boost::tuples::make_tuple(std::size_t(N),boost::any(data),true));\
        }\
        _KING_ch_any.Write(boost::tuples::make_tuple(std::size_t(N),boost::any(data),false));\
    });

#define KING_SELECT_CHANNEL_SWITCH \
    _KING_any_data_t _KING_any_data;\
    while(_KING_sum)\
    {\
        _KING_any_data_t data;\
        _KING_ch_any.Read(data);\
        switch(data.get<KING_SELECT_CHANNEL_INDEX_CASE>())\
        {

#define KING_SELECT_CHANNEL_CASE(CHAN,N,VAR,OPEN,CODE) \
    case N:\
    {\
    typedef _KING_##CHAN##_t::Data _KING_chan_data_t;\
    _KING_chan_data_t& VAR = boost::any_cast<_KING_chan_data_t&>(data.get<KING_SELECT_CHANNEL_INDEX_DATA>());\
    bool OPEN = data.get<KING_SELECT_CHANNEL_INDEX_OPEN>();\
    if(!OPEN){--_KING_sum;}\
    {CODE}\
    }\
    break;


#define KING_SELECT_CHANNEL_END } } }




#define KING_SELECT_CHANNEL(CHAN,VAR,OK,CLOSE,CODE) {\
    bool OK = false;\
    typedef decltype(CHAN) _KING_##CHAN##_t;\
    _KING_##CHAN##_t::Data VAR;\
    if(CHAN.TryRead(VAR,CLOSE))\
    {OK=true;}\
    CODE\
}
    };
};

#endif // KING_LIB_HEADER_THREAD_CHANNEL
