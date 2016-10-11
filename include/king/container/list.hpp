//一個同 std::list 的 鍊錶 用在某些不能使用 std::list 的位置
#ifndef KING_LIB_HEADER_CONTAINER_LIST
#define KING_LIB_HEADER_CONTAINER_LIST

#include <memory>
#include <vector>
#include <algorithm>


namespace king
{
namespace container
{
    template<typename T>
    class list
    {
    protected:
        typedef struct _NODE_{
            //節點 存儲 數據
            T Data;

            //上一個 節點 為NULL 表示 第一個節點
            _NODE_* Prev;

            //下一個 節點 為 NULL 表示 最後一個節點
            _NODE_* Next;
        }NODE,*PNODE;


        //鍊錶頭
        PNODE _first;
        //鍊錶尾
        PNODE _last;

        //節點 數量
        std::size_t _size;

    protected:
        PNODE create_node()
        {
            return new(std::nothrow) NODE;
        }
        void destory_node(PNODE p)
        {
            delete p;
        }
    public:
        list():_first(NULL),_last(NULL),_size(0)
        {

        }
        list(const list& copy):_first(NULL),_last(NULL),_size(0)
        {
            insert(begin(),copy.begin(),copy.end());

        }
        list& operator=(const list& copy)
        {
            clear_insert(copy.begin(),copy.end());

            return *this;
        }

        //返回 節點 數量
        inline std::size_t size()const
        {
            return _size;
        }
        //返回 鍊錶 是否為空
        inline bool empty()const
        {
            return !_size;
        }
        //在鍊錶尾 壓入數據
        void push_back(const T& v)    //throw std::bad_alloc
        {
            //申請新 節點
            PNODE p = create_node();
            if(!p)
            {
                throw std::bad_alloc();
            }

            //copy 數據
            p->Data = v;

            //設置 next
            p->Next = NULL;

            //設置 precv
            if(_last)
            {
                //連接 節點
                _last->Next = p;
                p->Prev = _last;
            }
            else
            {
                //_last 為空
                //則 鍊錶一定為空 _first 為空
                _first = p;
                p->Prev = NULL;

            }

            //設置鍊錶 尾
            _last = p;

            //增加數量記錄
            ++_size;
        }
        //返回鍊錶 尾節點
        //鍊錶為空 發生未定義行為
        inline T& back()
        {
            return _last->Data;
        }
        //彈出鍊錶 尾節點
        //鍊錶為空 發生未定義行為
        void pop_back()
        {
            PNODE p = _last;
            PNODE prev = _last->Prev;
            if(prev)
            {
                prev->Next = NULL;
            }
            else
            {
                //設置新 頭節點
                _first = NULL;
            }


            //設置新 尾節點
            _last = prev;


            //減少數量記錄
            --_size;

            //釋放資源
            destory_node(p);

        }

        //在鍊錶頭 壓入數據
        void push_front(const T& v)    //throw std::bad_alloc
        {
            //申請新 節點
            PNODE p = create_node();
            if(!p)
            {
                throw std::bad_alloc();
            }

            //copy 數據
            p->Data = v;

            //設置 prev
            p->Prev = NULL;

            //設置 next
            if(_first)
            {
                //連接 節點
                _first->Prev = p;
                p->Next = _first;
            }
            else
            {
                //_first 為空
                //則 鍊錶一定為空 _last 為空
                _last = p;
                p->Next = NULL;

            }

            //設置鍊錶 頭
            _first = p;

            //增加數量記錄
            ++_size;
        }
        //返回鍊錶 頭節點
        //鍊錶為空 發生未定義行為
        inline T& front()
        {
            return _first->Data;
        }

        //彈出鍊錶 頭節點
        //鍊錶為空 發生未定義行為
        void pop_front()
        {
            PNODE p = _first;
            PNODE next = _first->Next;
            if(next)
            {
                next->Prev = NULL;
            }
            else
            {
                //設置新 尾節點
                _last = NULL;
            }


            //設置新 頭節點
            _first = next;


            //減少數量記錄
            --_size;

            //釋放資源
            destory_node(p);

        }

        void clear()
        {
            PNODE next;
            while(_first)
            {
                next =_first->Next;
                try
                {
                    destory_node(_first);
                }
                catch(...)
                {

                }
                _first = next;
            }
            _size = 0;
            _last = NULL;
        }
    public:
        //迭代器定義
        class iterator
        {
        public:
            PNODE _p;
            const list* _list;

            iterator(PNODE p,const list* l):_p(p),_list(l)
            {

            }
            iterator(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            iterator& operator=(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            T* operator->()const
            {
                return &(_p->Data);
            }
            T& operator*()const
            {
                return _p->Data;
            }

            inline bool operator==(const iterator& compare)const
            {
                return _p == compare._p;
            }
            inline bool operator!=(const iterator& compare)const
            {
                return _p != compare._p;
            }

            inline iterator operator++()
            {
                _p = _p->Next;
                return *this;
            }
            inline iterator operator++(int)
            {
                iterator tmp = *this;
                ++*this;
                return tmp;
            }
            inline iterator operator--()
            {
                if(_p)
                {
                    _p = _p->Prev;
                }
                else
                {
                    _p = _list->_last;
                }
                return *this;
            }
            inline iterator operator--(int)
            {
                iterator tmp = *this;
                --*this;
                return tmp;
            }
        };

        //返回 正向 迭代器 頭
        inline iterator begin()
        {
            return iterator(_first,this);
        }

        //返回 正向 迭代器 尾
        inline iterator end()
        {
            return iterator(NULL,this);
        }


        class const_iterator
        {
        public:
            const list* _list;
            PNODE _p;

            const_iterator(PNODE p,const list* l):_p(p),_list(l)
            {

            }
            const_iterator(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            const_iterator& operator=(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            const_iterator(const const_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            const_iterator& operator=(const const_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            const T* operator->()const
            {
                return &(_p->Data);
            }
            const T& operator*()const
            {
                return _p->Data;
            }

            inline bool operator==(const const_iterator& compare)const
            {
                return _p == compare._p;
            }
            inline bool operator!=(const const_iterator& compare)const
            {
                return _p != compare._p;
            }

            inline const_iterator operator++()
            {
                _p = _p->Next;
                return *this;
            }
            inline const_iterator operator++(int)
            {
                const_iterator tmp = *this;
                ++*this;
                return tmp;
            }
            inline const_iterator operator--()
            {
                if(_p)
                {
                    _p = _p->Prev;
                }
                else
                {
                    _p = _last;
                }
                return *this;
            }
            inline const_iterator operator--(int)
            {
                const_iterator tmp = *this;
                --*this;
                return tmp;
            }
        };
        inline const_iterator begin()const
        {
            return const_iterator(_first,this);
        }
        inline const_iterator end()const
        {
            return const_iterator(NULL,this);
        }


        //逆向迭代器定義
        class reverse_iterator
        {
        public:
            PNODE _p;
            const list* _list;

            reverse_iterator(PNODE p,const list* l):_p(p),_list(l)
            {

            }

            reverse_iterator(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;

                if(_p == _list->_first)
                {
                    _p = NULL;
                }
                else if(_p == NULL)
                {
                    _p = _list->_last;
                }
            }
            iterator base()const
            {
                PNODE p = _p;
                if(p == NULL)
                {
                    p = _list->_first;
                }
                else if(p == _list->_last)
                {
                    p = NULL;
                }

                return iterator(p,_list);
            }

            reverse_iterator(const reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            reverse_iterator& operator=(const reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            T* operator->()const
            {
                return &(_p->Data);
            }
            T& operator*()const
            {
                return _p->Data;
            }

            inline bool operator==(const reverse_iterator& compare)const
            {
                return _p == compare._p;
            }
            inline bool operator!=(const reverse_iterator& compare)const
            {
                return _p != compare._p;
            }

            inline reverse_iterator operator++()
            {
                _p = _p->Prev;
                return *this;
            }
            inline reverse_iterator operator++(int)
            {
                reverse_iterator tmp = *this;
                ++*this;
                return tmp;
            }
            inline reverse_iterator operator--()
            {
                if(_p)
                {
                    _p = _p->Next;
                }
                else
                {
                    _p = _first;
                }
                return *this;
            }
            inline reverse_iterator operator--(int)
            {
                reverse_iterator tmp = *this;
                --*this;
                return tmp;
            }
        };
        //返回 逆向 迭代器 頭
        inline reverse_iterator rbegin()
        {
            return reverse_iterator(_last,this);
        }

        //返回 逆向 迭代器 尾
        inline reverse_iterator rend()
        {
            return reverse_iterator(NULL,this);
        }


        class const_reverse_iterator
        {
        public:
            PNODE _p;
            const list* _list;

            const_reverse_iterator(PNODE p,const list* l):_p(p),_list(l)
            {

            }

            const_reverse_iterator(const const_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;

                if(_p == _list->_first)
                {
                    _p = NULL;
                }
                else if(_p == NULL)
                {
                    _p = _list->_last;
                }
            }
            const_reverse_iterator(const iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;

                if(_p == _list->_first)
                {
                    _p = NULL;
                }
                else if(_p == NULL)
                {
                    _p = _list->_last;
                }
            }
            const_iterator base()const
            {
                PNODE p = _p;
                if(p == NULL)
                {
                    p = _list->_first;
                }
                else if(p == _list->_last)
                {
                    p = NULL;
                }

                return const_iterator(p,_list);
            }

            const_reverse_iterator(const reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            const_reverse_iterator& operator=(const reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            const_reverse_iterator(const const_reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
            }
            const_reverse_iterator& operator=(const const_reverse_iterator& copy)
            {
                _p = copy._p;
                _list = copy._list;
                return *this;
            }
            const T* operator->()const
            {
                return &(_p->Data);
            }
            const T& operator*()const
            {
                return _p->Data;
            }

            inline bool operator==(const const_reverse_iterator& compare)const
            {
                return _p == compare._p;
            }
            inline bool operator!=(const const_reverse_iterator& compare)const
            {
                return _p != compare._p;
            }

            inline const_reverse_iterator operator++()
            {
                _p = _p->Prev;
                return *this;
            }
            inline const_reverse_iterator operator++(int)
            {
                const_reverse_iterator tmp = *this;
                ++*this;
                return tmp;
            }
            inline const_reverse_iterator operator--()
            {
                if(_p)
                {
                    _p = _p->Next;
                }
                else
                {
                    _p = _first;
                }
                return *this;
            }
            inline const_reverse_iterator operator--(int)
            {
                const_reverse_iterator tmp = *this;
                --*this;
                return tmp;
            }
        };
        //返回 逆向 迭代器 頭
        inline const_reverse_iterator rbegin()const
        {
            return const_reverse_iterator(_last,this);
        }

        //返回 逆向 迭代器 尾
        inline const_reverse_iterator rend()const
        {
            return const_reverse_iterator(NULL,this);
        }

        //insert
        void insert(const_iterator where,const T& v) // throw std::bad_alloc
        {
            if(where == end())
            {
                push_back(v);
                return;
            }

            //申請新 節點
            PNODE p = create_node();
            if(!p)
            {
                throw std::bad_alloc();
            }

            //插入位置
            PNODE p_i = where._p;

            //copy 數據
            p->Data = v;

            //設置 next
            p->Next = p_i;
            PNODE prev = p_i->Prev;
            p_i->Prev = p;

            //設置 prev
            if(prev)
            {
                p->Prev = prev;
                prev->Next = p;
            }
            else
            {
                //prev 為空
                //where 為鍊錶頭
                _first = p;
                p->Prev = NULL;
            }

            //增加數量記錄
            ++_size;

        }
        void insert_n(const_iterator where,const std::size_t n,const T& v) // throw std::bad_alloc
        {
            if(!n)
            {
                return;
            }

            //申請新 節點
            std::pair<PNODE,PNODE> range = create_range(n,v);


            //連接節點
            join(where,range,n);

        }
        template<typename Iterator>
        void insert(const_iterator where,Iterator begin,Iterator end) //throw std::bad_alloc
        {
            //申請新 節點
            std::size_t n;
            std::pair<PNODE,PNODE> range = copy_range(begin,end,n);
            if(!n)//[begin end) 為空
            {
                return;
            }

            //連接節點
            join(where,range,n);

        }

    protected:
        template<typename Iterator>
        void clear_insert(Iterator begin,Iterator end) //throw std::bad_alloc
        {
            //申請新 節點
            std::size_t n;
            std::pair<PNODE,PNODE> range = copy_range(begin,end,n);
            if(!n)//[begin end) 為空
            {
                return;
            }

            clear();

            //連接節點
            join(this->begin(),range,n);

        }

        template<typename Iterator>
        std::pair<PNODE,PNODE> copy_range(Iterator begin,Iterator end,std::size_t& n) //throw std::bad_alloc
        {
            PNODE prev = NULL;
            PNODE first = NULL;

            n = 0;
            for(;begin!=end;++begin)
            {
                //申請新 節點
                PNODE p = create_node();
                if(!p)
                {
                    destory_range(first);
                    throw std::bad_alloc();
                }

                if(!first)
                {
                    first = p;
                }

                p->Data = *begin;
                p->Next = NULL;
                if(prev)
                {
                    prev->Next = p;
                }
                p->Prev = prev;

                prev = p;

                ++n;
            }
            return std::make_pair(first,prev);
        }
        void destory_range(PNODE first)
        {
            PNODE next;
            while(first)
            {
                next = first->Next;

                destory_node(first);

                first = next;
            }
        }

        std::pair<PNODE,PNODE> create_range(std::size_t n,const T& v) //throw std::bad_alloc
        {
            PNODE prev = NULL;
            PNODE first = NULL;

            while(n)
            {
                //申請新 節點
                PNODE p = create_node();
                if(!p)
                {
                    destory_range(first);
                    throw std::bad_alloc();
                }

                if(!first)
                {
                    first = p;
                }

                p->Data = v;
                p->Next = NULL;
                if(prev)
                {
                    prev->Next = p;
                }
                p->Prev = prev;

                prev = p;

                --n;
            }
            return std::make_pair(first,prev);
        }
        std::pair<PNODE,PNODE> create_range(std::size_t n) //throw std::bad_alloc
        {
            PNODE prev = NULL;
            PNODE first = NULL;

            while(n)
            {
                //申請新 節點
                PNODE p = create_node();
                if(!p)
                {
                    destory_range(first);
                    throw std::bad_alloc();
                }

                if(!first)
                {
                    first = p;
                }

                //p->Data = v;
                p->Next = NULL;
                if(prev)
                {
                    prev->Next = p;
                }
                p->Prev = prev;

                prev = p;

                --n;
            }
            return std::make_pair(first,prev);
        }
        void insert_n(const_iterator where,const std::size_t n) // throw std::bad_alloc
        {
            if(!n)
            {
                return;
            }

            //申請新 節點
            std::pair<PNODE,PNODE> range = create_range(n);


            //連接節點
            join(where,range,n);

        }
        void join(const_iterator where, std::pair<PNODE,PNODE> range,std::size_t n)
        {
            //push back
            if(where == this->end())
            {
                if(_last)
                {
                    _last->Next = range.first;
                    range.first->Prev = _last;

                    _last = range.second;
                    _size += n;
                }
                else
                {
                    //_last 不存在 鍊錶為空
                    _first = range.first;
                    _last = range.second;
                    _size = n;
                }
                return;
            }

            //插入位置
            PNODE p_i = where._p;

            //設置 next
            PNODE p = range.second;
            p->Next = p_i;
            PNODE prev = p_i->Prev;
            p_i->Prev = p;

            //設置 prev
            p = range.first;
            if(prev)
            {
                p->Prev = prev;
                prev->Next = p;
            }
            else
            {
                //prev 為空
                //where 為鍊錶頭
                _first = p;
                p->Prev = NULL;
            }

            //增加數量記錄
            _size += n;
        }
    public:
        //swap
        inline void swap(list& right)
        {
            std::swap(_first,right._first);
            std::swap(_last,right._last);
            std::swap(_size,right._size);
        }
        //reverse
        void reverse()
        {
            PNODE pos = _first;
            PNODE next;
            PNODE prev = NULL;
            while(pos)
            {
                next = pos->Next;

                pos->Prev = prev;
                pos->Next = NULL;

                prev = pos;
                pos = next;
            }
            std::swap(_first,_last);
        }

        //erase
        iterator erase(const_iterator where)
        {
            PNODE p = where._p;
            PNODE next = p->Next;
            PNODE prev = p->Prev;

            if(next)
            {
                next->Prev = prev;
            }
            else
            {
                //設置新的 尾節點
                _last = prev;
            }


            if(prev)
            {
                prev->Next = next;
            }
            else
            {
                //設置新的 頭節點
                _first = next;
            }


            //減少數量記錄
            --_size;

            return iterator(next,this);
        }
        iterator erase(const_iterator begin,const_iterator end)
        {
            if(begin == end)
            {
                return iterator(begin._p,this);
            }

            std::size_t n = 0;

            PNODE p_end = end._p;
            PNODE p_begin = begin._p;


            PNODE next = p_end;
            PNODE prev = p_begin->Prev;

            destory_range(p_begin,p_end);

            if(next)
            {
                next->Prev = prev;
            }
            else
            {
                //設置新的 尾節點
                _last = prev;
            }


            if(prev)
            {
                prev->Next = next;
            }
            else
            {
                //設置新的 頭節點
                _first = next;
            }


            //減少數量記錄
            _size -= n;

            return iterator(p_end,this);
        }

    protected:
        void destory_range(PNODE begin,PNODE end)
        {
            PNODE next;
            while(begin != end /*&& begin*/)
            {
                next = begin->Next;

                destory_node(begin);

                begin = next;
            }
        }
    public:
        //remove
        void remove(const T& v)
        {
            iterator iter = begin();
            while(iter != end())
            {
                if(*iter == v)
                {
                    iter = erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }

        template<typename Compare>
        void remove_if(Compare compare)
        {
            iterator iter = begin();
            while(iter != end())
            {
                if(compare(*iter))
                {
                    iter = erase(iter);
                }
                else
                {
                    ++iter;
                }
            }
        }


        //sort
        void sort() //throw std::bad_alloc
        {
            //sort
            std::vector<T> v(_size);
            std::size_t i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                v[i++] = *iter;
            }
            std::sort(v.begin(),v.end());


            //set
            i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                *iter = v[i++];
            }
        }
        template<typename Compare>
        void sort(Compare compare) //throw std::bad_alloc
        {
            //sort
            std::vector<T> v(_size);
            std::size_t i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                v[i++] = *iter;
            }
            std::sort(v.begin(),v.end(),compare);


            //set
            i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                *iter = v[i++];
            }
        }

        //merge and sort
        //right will clear
        void merge(list& right) //throw std::bad_alloc
        {
            //sort
            std::vector<T> v(_size + right.size());
            std::size_t i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                v[i++] = *iter;
            }
            for(iterator iter = right.begin();iter!=right.end();++iter)
            {
                v[i++] = *iter;
            }
            std::sort(v.begin(),v.end());

            //insert
            insert_n(end(),right.size());
            right.clear();

            //set
            typename std::vector<T>::const_iterator iter_v = v.begin();
            for(iterator iter = begin();iter!=end();++iter)
            {
                *iter = *iter_v;
                ++iter_v;
            }

        }
        template<typename Compare>
        void merge(list& right,Compare compare) //throw std::bad_alloc
        {
            //sort
            std::vector<T> v(_size + right.size());
            std::size_t i = 0;
            for(iterator iter = begin();iter!=end();++iter)
            {
                v[i++] = *iter;
            }
            for(iterator iter = right.begin();iter!=right.end();++iter)
            {
                v[i++] = *iter;
            }
            std::sort(v.begin(),v.end(),compare);

            //insert
            insert_n(end(),right.size());
            right.clear();

            //set
            typename std::vector<T>::const_iterator iter_v = v.begin();
            for(iterator iter = begin();iter!=end();++iter)
            {
                *iter = *iter_v;
                ++iter_v;
            }

        }

    };


};
};

#endif // KING_LIB_HEADER_CONTAINER_LIST
