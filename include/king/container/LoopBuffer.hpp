/*  一個固定大小的 環形緩衝區

    要求 :
        緩存數據 必須滿足 copy 語義
        緩存數據 必須存在 無參 T() 構造函數
*/
#ifndef KING_LIB_HEADER_CONTAINER_LOOP_BUFFER
#define KING_LIB_HEADER_CONTAINER_LOOP_BUFFER


#include <memory>
#include <array>
#include <type_traits>
#include <king/Exception.hpp>

namespace king
{
namespace container
{
    class BadLoopBufferWrite
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::container::BadLoopBufferWrite";

        }
    };
    class BadLoopBufferRead
        :public king::Exception
    {
    public:
        virtual const char* What()
        {
            return "king::container::BadLoopBufferRead";
        }
    };

    template<typename T,std::size_t N>
    class LoopBuffer
    {
    public:
        explicit LoopBuffer()
        {
            static_assert(N>0,"N must larage 0");
        }
        virtual ~LoopBuffer(){}
    protected:
        LoopBuffer(const LoopBuffer& copy) = delete;
        LoopBuffer& operator=(const LoopBuffer& copy) = delete;

    public:
        //緩衝區
        std::array<T,N> _buffer;
        //有效 數據 起始位置
        std::size_t _begin = 0;
        //有效 數據 結束位置
        std::size_t _end = 0;
    public:
        //有效 數據 量
        std::size_t _n = 0;
    public:
        //返回 緩衝區 最大容納 數據量
        inline std::size_t Capacity()const
        {
            return N;
        }

        //返回 有效 數據 量
        inline std::size_t Size()const
        {
            return _n;
        }

        //返回 有效 數據 是否為空
        inline bool Empty()const
        {
            return _n == 0;
        }

        //返回 有效 數據 是否存在
        inline operator bool()const
        {
            return _n != 0;
        }

        //返回 空閒 容量
        inline std::size_t Free()const
        {
            return N - _n;
        }

        //返回是否 可寫
        inline bool IsWrite()const
        {
            return N != _n;
        }
        //返回是否 可讀
        inline bool IsRead()const
        {
            return _n != 0;
        }


        //寫入數據
        LoopBuffer& Write(const T& v) //throw BadLoopBufferWrite
        {
            if(!IsWrite())
            {
                throw BadLoopBufferWrite();
            }

            _buffer[_end++] = v;
            ++_n;
            if(_end == N)
            {
                _end = 0;
            }

            return *this;
        }
        inline LoopBuffer& operator<<(const T& v)//throw BadLoopBufferWrite
        {
            return Write(v);
        }
        bool TryWrite(const T& v)//no throw
        {
            if(!IsWrite())
            {
                return false;
            }

            _buffer[_end++] = v;
            ++_n;
            if(_end == N)
            {
                _end = 0;
            }
            return true;
        }

        //讀取數據
        LoopBuffer& Read(T& v) //throw BadLoopBufferRead
        {
            if(!IsRead())
            {
                throw BadLoopBufferRead();
            }

            v = _buffer[_begin];
            _buffer[_begin++] = T();
            --_n;
            if(_begin == N)
            {
                _begin = 0;
            }
            return *this;
        }
        inline LoopBuffer& operator>>(T& v)//throw BadLoopBufferRead
        {
            return Read(v);
        }
        bool TryRead(T& v) //no throw
        {
            if(!IsRead())
            {
                return false;
            }

            v = _buffer[_begin];
            _buffer[_begin++] = T();
            --_n;
            if(_begin == N)
            {
                _begin = 0;
            }
            return true;
        }


    public:
        class Iterator
        {
        protected:
            typedef LoopBuffer<T,N> Container;
        public:
            Container* _ptr = NULL;
            std::size_t _pos = 0;
        public:
            explicit Iterator(std::size_t pos,LoopBuffer<T,N>* ptr):_ptr(ptr),_pos(pos)
            {

            }
            Iterator(const Iterator& copy)
            {
                _ptr = copy._ptr;
                _pos = copy._pos;
            }
            Iterator& operator=(const Iterator& copy)
            {
                _ptr = copy._ptr;
                _pos = copy._pos;
                return *this;
            }

            T* operator->()const
            {
                return &(_ptr->_buffer[_pos]);
            }
            T& operator*()const
            {
                return _ptr->_buffer[_pos];
            }

            inline bool operator==(const Iterator& compare)const
            {
                return _pos == compare._pos && _ptr == compare._ptr;
            }
            inline bool operator!=(const Iterator& compare)const
            {
                return _pos != compare._pos || _ptr != compare._ptr;
            }

            inline Iterator operator++()
            {
                ++_pos;
                if(_pos == N)
                {
                    _pos = 0;
                }
                if(_pos == _ptr->_end)
                {
                    _pos = N;
                }
                return *this;
            }
            inline Iterator operator++(int)
            {
                Iterator tmp = *this;
                ++*this;
                return tmp;
            }

            inline Iterator operator--()
            {
                if(_pos == N)
                {
                   _pos = _ptr->_end;
                }
                else
                {
                    if(_pos == 0)
                    {
                        _pos = N - 1;
                    }
                    else
                    {
                        --_pos;
                    }
                }
                return *this;
            }
            inline Iterator operator--(int)
            {
                Iterator tmp = *this;
                --*this;
                return tmp;
            }

        };

        //迭代器定義 用於遍歷已緩存數據
        inline Iterator Begin()
        {
            return Iterator(0,this);
        }
        inline Iterator End()
        {
            return Iterator(N,this);
        }
        inline Iterator begin()
        {
            return Iterator(0,this);
        }
        inline Iterator end()
        {
            return Iterator(N,this);
        }

        class ConstIterator
        {
        protected:
            typedef LoopBuffer<T,N> Container;
            Container* _ptr = NULL;
            std::size_t _pos = 0;
        public:
            explicit ConstIterator(std::size_t pos,LoopBuffer<T,N>* ptr):_ptr(ptr),_pos(pos)
            {

            }
            ConstIterator(const Iterator& copy):_ptr(copy._ptr),_pos(copy._pos)
            {

            }
            ConstIterator(const ConstIterator& copy)
            {
                _ptr = copy._ptr;
                _pos = copy._pos;
            }
            ConstIterator& operator=(const ConstIterator& copy)
            {
                _ptr = copy._ptr;
                _pos = copy._pos;
                return *this;
            }

            T* operator->()const
            {
                return &(_ptr->_buffer[_pos]);
            }
            T& operator*()const
            {
                return _ptr->_buffer[_pos];
            }

            inline bool operator==(const ConstIterator& compare)const
            {
                return _pos == compare._pos && _ptr == compare._ptr;
            }
            inline bool operator!=(const ConstIterator& compare)const
            {
                return _pos != compare._pos || _ptr != compare._ptr;
            }

            inline ConstIterator operator++()
            {
                ++_pos;
                if(_pos == N)
                {
                    _pos = 0;
                }
                if(_pos == _ptr->_end)
                {
                    _pos = N;
                }
                return *this;
            }
            inline ConstIterator operator++(int)
            {
                ConstIterator tmp = *this;
                ++*this;
                return tmp;
            }

            inline ConstIterator operator--()
            {
                if(_pos == N)
                {
                   _pos = _ptr->_end;
                }
                else
                {
                    if(_pos == 0)
                    {
                        _pos = N - 1;
                    }
                    else
                    {
                        --_pos;
                    }
                }
                return *this;
            }
            inline ConstIterator operator--(int)
            {
                Iterator tmp = *this;
                --*this;
                return tmp;
            }

        };

        //迭代器定義 用於遍歷已緩存數據
        inline ConstIterator Begin()const
        {
            return ConstIterator(0,this);
        }
        inline ConstIterator End()const
        {
            return ConstIterator(N,this);
        }
        inline ConstIterator begin()const
        {
            return ConstIterator(0,this);
        }
        inline ConstIterator end()const
        {
            return ConstIterator(N,this);
        }


    };


};
};

#endif // KING_LIB_HEADER_CONTAINER_LOOP_BUFFER
