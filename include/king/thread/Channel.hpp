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
    ... other channel
    KING_SELECT_CHANNEL_SWITCH
    KING_SELECT_CHANNEL_CASE(ch0,0,v0,open,
    {
        if(open)
        { do work by v0}
        else
        { do close}
    })
    KING_SELECT_CHANNEL_CASE(ch1,1,v1,open,
    {
        if(open)
        { do work by v1}
        else
        { do close}
    })
    ... other channel
    KING_SELECT_CHANNEL_END

    KING_SELECT_CHANNEL_BEGIN KING_SELECT_CHANNEL_END 內部 使用一個 while(true) 等待 任何 channel 的 數據通知 並且轉發到用戶代碼
    直到 所有 channel 都 關閉 才會 退出 KING_SELECT_CHANNEL_BEGIN KING_SELECT_CHANNEL_END 循環
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
    };
};

#endif // KING_LIB_HEADER_THREAD_CHANNEL
