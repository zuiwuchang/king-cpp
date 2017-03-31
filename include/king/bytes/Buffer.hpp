//需要用到的 型別 定義
#ifndef KING_LIB_HEADER_BYTES_BUFFER
#define KING_LIB_HEADER_BYTES_BUFFER

#include <list>

#include <king/bytes/type.hpp>
namespace king
{
namespace bytes
{
    
/**
*   \brief king::Byte 流緩衝區
*
*   類似 golang的bytes.Buffer提供了一個 Byte 的 讀寫流緩衝區
*/
class Buffer
{
public:
    /** 
    *    \brief 字節定義
    */
    typedef king::Byte Byte;
    /** 
    *   \brief 分片定義
    */
    typedef std::shared_ptr<Fragmentation> FragmentationSpt;
protected:
    /** 
    *   \brief 創建分片時 分片的參考大小
    */
    int _capacity;

    /** 
    *   \brief 分片 緩存
    */
    std::list<FragmentationSpt> _fragmentations;

    /** 
    *   \brief 緩存一個 分片 以便 可以重複利用
    */
    FragmentationSpt _cache;
public:
     /** 
    *   \brief 構造一個 緩衝區
    *   \param capacity 當需要創建新分片時 分片參考大小
    */
    explicit Buffer(int capacity = 1024):_capacity(capacity)
    {
    }

    /** 
    *   \brief 不可拷貝
    */
    Buffer& operator=(const Buffer&) = delete;
    /** 
    *   \brief 不可拷貝
    */
    Buffer(const Buffer&) = delete;

    /** 
    *   \brief 清空流中的 數據
    *   \param clearcache  是否刪除 this._cache
    */
    void Reset(bool clearcache = true)
    {
        if(clearcache)
        {
            _cache.reset();
        }
        else
        {
            if(!_cache && !_fragmentations.empty())
            {
                _cache = _fragmentations.back();
            }
        }
        _fragmentations.clear();
    }

    /** 
    *   \brief 刪除 this._cache
    */
    inline void ResetCache()
    {
         _cache.reset();
    }

    /** 
    *   \brief 返回 流中 待讀字節數
    */
    std::size_t Size()const
    {
       std::size_t sum = 0;
       for(const FragmentationSpt& f:_fragmentations)
       {
           sum += f->Size();
       }
       return sum;
    }

    /** 
    *   \brief 向流中 寫入數據
    *
    *   如果 發生任何 錯誤 將 不寫入 數據 並返回 0
    *   \param bytes    待 寫入字節指針
    *   \param n    寫入長度
    *   \return 實際寫入長度 
    */
    std::size_t Write(const Byte* bytes,const std::size_t n)
    {
        if(!n)
        {
            return 0;
        }

        if (_fragmentations.empty())
        {
            //1次 寫入
            FragmentationSpt f = createFragmentation(n);
            if(!f)
            {
                //創建 分片 失敗
                return 0;
            }

            try
            {
                //添加新 分片
                _fragmentations.push_back(f);

                //寫入 分片
                f->Write(bytes,n);
            }
            catch(const std::bad_alloc&)
            {
                return 0;
            }

            return n;
        }

        FragmentationSpt& f0 = _fragmentations.back();
        std::size_t free = f0->GetFree();
        if(free >= n)
        {
            //1次 寫入
            f0->Write(bytes,n);
            return n;
        }

        //2次 寫入
        std::size_t need = n - free;
        std::size_t capacity = _capacity;
        if(capacity < need)
        {
            capacity = need;
        }
        //創建 新分片
        FragmentationSpt f = createFragmentation(capacity);
        if(!f)
        {
            //創建 分片 失敗
            return 0;
        }
        try
        {
            //添加新 分片
            _fragmentations.push_back(f);

            //寫入 數據
            f0->Write(bytes,free);
            f->Write(bytes + free,need);
        }
        catch(const std::bad_alloc&)
        {
            return 0;
        }

        return n;
    }
protected:
    /** 
    *   \brief 創建 一個大小至少為 capacity 的分片
    *
    *   \param capacity  分片參考大小
    */
    FragmentationSpt createFragmentation(const std::size_t capacity)
    {
        try
        {
            if(!_cache)
            {
                return std::make_shared<Fragmentation>(capacity);
            }
            if(_cache->Capacity() < capacity)
            {
                return std::make_shared<Fragmentation>(capacity);
            }
        }
        catch(const std::bad_alloc&)
        {
            return FragmentationSpt();
        }

        FragmentationSpt f = _cache;
        f->Init();
        _cache.reset();

        return f;
    }

public:
     /** 
    *   \brief 將緩衝區 copy 到指定內存 返回實際 copy數據長
    *
    *   被copy的數據 不會從 緩衝區中 刪除\n
    *   如果 n > 緩衝區 數據 將 只copy 緩衝區 否則 copy n 字節數據
    *
    *   \param bytes    待 讀緩衝區
    *   \param n    緩衝區長度
    *   \return 返回實際 copy數據長
    */
    std::size_t CopyTo(Byte* bytes,std::size_t n)const
    {
        std::size_t sum = 0;
        std::size_t count;
        for(const FragmentationSpt& f:_fragmentations)
        {
            count = f->CopyTo(bytes,n);
            n -= count;
            bytes += count;
            sum += count;

            if(n < 1)
            {
                break;
            }
        }
        return sum;
    }
    /** 
    *   \brief 將緩衝區 copy 到指定內存 返回實際 copy數據長
    *
    *   被copy的數據 不會從 緩衝區中 刪除\n
    *   如果 n > 緩衝區 數據 將 只copy 緩衝區 否則 copy n 字節數據
    *
    *   \param skip    忽略 Buffer 中前skip個字節
    *   \param bytes    待 讀緩衝區
    *   \param n    緩衝區長度
    *   \return 返回實際 copy數據長
    */
    std::size_t CopyTo(std::size_t skip,Byte* bytes,std::size_t n)const
    {
        std::size_t sum = 0;
        std::size_t need_skip = 0;
        std::size_t size;
        std::size_t count;
        for(const FragmentationSpt& f:_fragmentations)
        {
            if(skip)
            {
                need_skip = skip;

                size = f->Size();
                if(need_skip > size)
                {
                    need_skip = size;
                }
                skip -= need_skip;

            }
            if(need_skip)
            {
                count = f->CopyTo(need_skip,bytes,n);
            }
            else
            {
                count = f->CopyTo(bytes,n);
            }
            n -= count;
            bytes += count;
            sum += count;

            if(n < 1)
            {
                break;
            }
        }
        return sum;
    }


    /** 
    *   \brief 從流中 讀取數據 被讀取的數據 將被刪除
    *
    *   \param bytes    待 讀緩衝區
    *   \param n    緩衝區長度
    *   \return 實際 讀取數據流 長度
    */
    std::size_t Read(Byte* bytes,std::size_t n)
    {
        std::size_t sum = 0;
        std::size_t count;
        while(n)
        {
            //返回 可讀 分片
            FragmentationSpt f = getReadFragmentation();
            if(!f)
            {
                break;
            }
            count = f->Read(bytes,n);
            n -= count;
            bytes += count;
            sum += count;
        }
        removeNoReadFragmentation();

        return sum;
    }
protected:
    /** 
    *   \brief 返回當前 可讀 緩衝區 或 空指針
    */
    FragmentationSpt getReadFragmentation()
    {
        while(!_fragmentations.empty())
        {
            const FragmentationSpt& f = _fragmentations.front();
            if(f->Size() < 1)
            {
                cacheFragmentation(f);
                _fragmentations.pop_front();
                continue;
            }
            return f;
        }

        return FragmentationSpt();
    }
    /** 
    *   \brief 將 f 設置爲 緩存
    */
    inline void cacheFragmentation(const FragmentationSpt& f)
    {
        if(!_cache || _cache->Size() < f->Size())
        {
            _cache = f;
        }
    }

    /** 
    *   \brief 刪除 無數據可讀的 分片
    */
    void removeNoReadFragmentation()
    {
        while(!_fragmentations.empty())
        {
            const FragmentationSpt& f = _fragmentations.front();
            if(f->Size())
            {
                break;
            }
            cacheFragmentation(f);
            _fragmentations.pop_front();
        }
    }
};




//為兼容舊風格 的保留代碼
//一個類似 golang bytes.Buffer 的 io 緩衝區
class buffer_t
{
public:
    typedef king::byte_t byte_t;
    typedef std::shared_ptr<fragmentation_t> fragmentation_spt;
protected:
    //參考分片 大小
    int _capacity;

    //分片 緩存
    std::list<fragmentation_spt> _fragmentations;

    //緩存一個 分片 以便 可以重複利用
	fragmentation_spt _cache;
public:
    explicit buffer_t(int capacity = 1024):_capacity(capacity)
    {
    }
    buffer_t& operator=(const buffer_t&) = delete;
    buffer_t(const buffer_t&) = delete;

    //清空流中的 數據
    //clearcache 是否刪除 _cache
    void reset(bool clearcache = true)
    {
        if(clearcache)
        {
            _cache.reset();
        }
        else
        {
            if(!_cache && !_fragmentations.empty())
            {
                _cache = _fragmentations.back();
            }
        }
        _fragmentations.clear();
    }
    //刪除 _cache
    inline void reset_cache()
    {
         _cache.reset();
    }

    //返回 流中 待讀字節數
    std::size_t size()const
    {
       std::size_t sum = 0;
       for(const fragmentation_spt& f:_fragmentations)
       {
           sum += f->size();
       }
       return sum;
    }

    //向流中 寫入數據 返回實際寫入 數據長度
    //如果 發生任何 錯誤 將 不寫入 數據 並返回 0
    std::size_t write(const byte_t* bytes,const std::size_t n)
    {
        if(!n)
        {
            return 0;
        }

        if (_fragmentations.empty())
        {
            //1次 寫入
            fragmentation_spt f = create_fragmentation(n);
            if(!f)
            {
                //創建 分片 失敗
                return 0;
            }

            try
            {
                //添加新 分片
                _fragmentations.push_back(f);

                //寫入 分片
                f->write(bytes,n);
            }
            catch(const std::bad_alloc&)
            {
                return 0;
            }

            return n;
        }

        fragmentation_spt& f0 = _fragmentations.back();
        std::size_t free = f0->get_free();
        if(free >= n)
        {
            //1次 寫入
            f0->write(bytes,n);
            return n;
        }

        //2次 寫入
        std::size_t need = n - free;
        std::size_t capacity = _capacity;
        if(capacity < need)
        {
            capacity = need;
        }
        //創建 新分片
        fragmentation_spt f = create_fragmentation(capacity);
        if(!f)
        {
            //創建 分片 失敗
            return 0;
        }
        try
        {
            //添加新 分片
            _fragmentations.push_back(f);

            //寫入 數據
            f0->write(bytes,free);
            f->write(bytes + free,need);
        }
        catch(const std::bad_alloc&)
        {
            return 0;
        }

        return n;
    }
protected:
    //創建 一個大小至少為 capacity 的分片
    fragmentation_spt create_fragmentation(const std::size_t capacity)
    {
        try
        {
            if(!_cache)
            {
                return std::make_shared<fragmentation_t>(capacity);
            }
            if(_cache->capacity() < capacity)
            {
                return std::make_shared<fragmentation_t>(capacity);
            }
        }
        catch(const std::bad_alloc&)
        {
            return fragmentation_spt();
        }

        fragmentation_spt f = _cache;
        f->init();
        _cache.reset();

        return f;
    }

public:
    //將緩衝區 copy 到指定內存 返回實際 copy數據長
    //被copy的數據 不會從 緩衝區中 刪除
    //如果 n > 緩衝區 數據 將 只copy 緩衝區 否則 copy n 字節數據
    std::size_t copy_to(byte_t* bytes,std::size_t n)const
    {
        std::size_t sum = 0;
        std::size_t count;
        for(const fragmentation_spt& f:_fragmentations)
        {
            count = f->copy_to(bytes,n);
            n -= count;
            bytes += count;
            sum += count;

            if(n < 1)
            {
                break;
            }
        }
        return sum;
    }
    std::size_t copy_to(std::size_t skip,byte_t* bytes,std::size_t n)const
    {
        std::size_t sum = 0;
        std::size_t need_skip = 0;
        std::size_t size;
        std::size_t count;
        for(const fragmentation_spt& f:_fragmentations)
        {
            if(skip)
            {
                need_skip = skip;

                size = f->size();
                if(need_skip > size)
                {
                    need_skip = size;
                }
                skip -= need_skip;

            }
            if(need_skip)
            {
                count = f->copy_to(need_skip,bytes,n);
            }
            else
            {
                count = f->copy_to(bytes,n);
            }
            n -= count;
            bytes += count;
            sum += count;

            if(n < 1)
            {
                break;
            }
        }
        return sum;
    }


    //從流中 讀取數據 被讀取的數據 將被刪除
    //返回 實際 讀取數據流
    std::size_t read(byte_t* bytes,std::size_t n)
    {
        std::size_t sum = 0;
        std::size_t count;
        while(n)
        {
            //返回 可讀 分片
            fragmentation_spt f = get_read_fragmentation();
            if(!f)
            {
                break;
            }
            count = f->read(bytes,n);
            n -= count;
            bytes += count;
            sum += count;
        }
        remove_no_read_fragmentation();

        return sum;
    }
protected:
    fragmentation_spt get_read_fragmentation()
    {
        while(!_fragmentations.empty())
        {
            const fragmentation_spt& f = _fragmentations.front();
            if(f->size() < 1)
            {
                cache_fragmentation(f);
                _fragmentations.pop_front();
                continue;
            }
            return f;
        }

        return fragmentation_spt();
    }
    //設置 緩存
    inline void cache_fragmentation(const fragmentation_spt& f)
    {
        if(!_cache || _cache->size() < f->size())
        {
            _cache = f;
        }
    }

    //刪除 無數據可讀的 分片
    void remove_no_read_fragmentation()
    {
        while(!_fragmentations.empty())
        {
            const fragmentation_spt& f = _fragmentations.front();
            if(f->size())
            {
                break;
            }
            cache_fragmentation(f);
            _fragmentations.pop_front();
        }
    }
};


};
};

#endif // KING_LIB_HEADER_BYTES_BUFFER
