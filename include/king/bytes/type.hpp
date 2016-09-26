//需要用到的 型別 定義
#ifndef KING_LIB_HEADER_BYTES_TYPE
#define KING_LIB_HEADER_BYTES_TYPE

#include <king/core.hpp>
namespace king
{
namespace bytes
{


//字節數組
class bytes_t
{
public:
    //創建大小為 size 的 字節數組
    explicit bytes_t(const std::size_t size)//no throw
    {
        _bytes = new(std::nothrow) king::byte_t[size];
        if(_bytes)
        {
            _size = size;
        }
    }
    //move
    bytes_t(bytes_t&& m)
    {
        _bytes = m._bytes;
        _size = m._size;

        m._bytes = nullptr;
        m._size = 0;
    }
    //move
    bytes_t& operator=(bytes_t&& m)
    {
        if(_bytes)
        {
            delete[] _bytes;
        }

        _bytes = m._bytes;
        _size = m._size;

        m._bytes = nullptr;
        m._size = 0;

        return *this;
    }

    bytes_t& operator=(const bytes_t&) = delete;
    bytes_t(const bytes_t&) = delete;
    virtual ~bytes_t()
    {
        if(_bytes)
        {
            delete[] _bytes;
        }
    }
    //返回數組 是否不為 空
    explicit inline operator bool()const
    {
        return _size != 0;
    }
    //返回數組 是否為空
    inline bool empty()const
    {
        return _size == 0;
    }

    //返回 數組 指針
    inline king::byte_t* get()const
    {
        return _bytes;
    }
    //返回 數組 指針
    inline operator king::byte_t*()const
    {
        return _bytes;
    }

    //返回數組 大小
    inline std::size_t size()const
    {
        return _size;
    }

    //釋放 數組
    void reset()
    {
        if(_bytes)
        {
            delete[] _bytes;
            _bytes = nullptr;
            _size = 0;
        }
    }
private:
    king::byte_t* _bytes = nullptr;
    std::size_t _size = 0;
};


//數據 分片
class fragmentation_t
{
protected:
    typedef king::byte_t byte_t;

     //分片數據
    std::shared_ptr<bytes_t> _array;
    std::size_t _capacity = 0;
    std::size_t _offset = 0;
    std::size_t _size = 0;

public:

    //創建 大小為 size 的分片
    explicit fragmentation_t(const std::size_t size):_capacity(size)
    {
        _array = std::make_shared<bytes_t>(size);
    }
    fragmentation_t(const fragmentation_t& copy) = delete;
    fragmentation_t& operator=(const fragmentation_t& copy) = delete;

    //返回 分片 是否不為空
    inline explicit operator bool()const
    {
        return _capacity != 0;
    }
    //返回 分片 是否為空
    inline bool empty()const
    {
        return _capacity == 0;
    }

    //重置 分片
    inline void init()
    {
        _offset = _size = 0;
    }
    //釋放 分片
    inline void reset()
    {
        _array.reset();
        _capacity = _offset = _size = 0;
    }

    //返回 容量
    inline std::size_t capacity() const
    {
        return _capacity;
    }
    //返回 有效數據 實際大小
    inline std::size_t size() const
    {
        return _size;
    }
    //返回 空閒 容量
    inline std::size_t get_free()
    {
        return _capacity - _offset - _size;
    }

    //寫入數據 返回實際寫入量
    std::size_t write(const byte_t* bytes,const std::size_t n)
    {
        std::size_t free = get_free();
        std::size_t need = n;
        if(need > free)
        {
            need = free;
        }
        memcpy(_array->get() + _offset + _size,bytes,need * sizeof(byte_t));
        _size += need;

        return need;
    }

    //讀取 數據 返回實際讀取 量
    //被讀取的 數據 將被 移除 緩衝區
    std::size_t read(byte_t* bytes,const std::size_t n)
    {
        std::size_t need = n;
        if(need > _size)
        {
            need = _size;
        }

        memcpy(bytes,_array->get() + _offset,need * sizeof(byte_t));
        _size -= need;
        _offset += need;

        return need;
    }

    //只 copy 數據 不 刪除緩衝區
    std::size_t copy_to(byte_t* bytes,const std::size_t n)const
    {
        std::size_t need = n;
        if (n > _size)
        {
            need = _size;
        }
        memcpy(bytes,_array->get() + _offset,need * sizeof(byte_t));

        return need;
    }
    //跳過n字節 copy
    std::size_t copy_to(const std::size_t skip,byte_t* bytes,const std::size_t n)const
    {
        if(skip >= _size)
        {
            return 0;
        }
        std::size_t offset = _offset + skip;
        std::size_t size = _size - skip;

        std::size_t need = n;
        if (need > size)
        {
            need = size;
        }
        memcpy(bytes,_array->get() + offset,need * sizeof(byte_t));

        return need;
    }
};


};
};

#endif // KING_LIB_HEADER_BYTES_TYPE
