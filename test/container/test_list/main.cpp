#include <gtest/gtest.h>

#include <iostream>

#include <king/container/list.hpp>
typedef king::container::list<int> list;


int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();

}

template<typename T>
void show_container(const T& container)
{
	for(typename T::const_iterator iter = container.begin();iter!=container.end();++iter)
	{
		std::cout<<*iter<<" ";
	}
	std::cout<<"\n";
}

TEST(ListBaseTest, HandleNoneZeroInput)
{
    list c;

    for(std::size_t i=0;i<10;++i)
    {
        c.push_back(i);
    }

    //iterator
    for(int& v:c)
    {
        v += 100;
    }
    int i = 0;
    for(list::iterator iter = c.begin();iter!=c.end();iter++)
    {
        int v = *iter;
        *iter -= 100;
        EXPECT_EQ(v,100+i);
        ++i;
    }
    i = 0;
    for(list::const_iterator iter = c.begin();iter!=c.end();++iter)
    {
        int v = *iter;
        EXPECT_EQ(v,i);
        ++i;
    }

	//reverse_iterator
	i = c.size();
	--i;
    for(list::reverse_iterator iter = c.rbegin();iter!=c.rend();iter++)
    {
        int v = *iter;
        EXPECT_EQ(v,i);
        --i;
    }

    //push pop
    for(int i=0;i<10;++i)
    {
        c.push_front(-i-1);
    }
    EXPECT_EQ(c.size(),20);
    i = -10;
    for(list::iterator iter = c.begin();iter!=c.end();iter++)
    {
        int v = *iter;
        EXPECT_EQ(v,i);
        ++i;
    }
    c.pop_back();
    EXPECT_EQ(c.back(),8);
    c.pop_front();
    EXPECT_EQ(c.front(),-9);
    EXPECT_EQ(c.size(),18);
    i = -9;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }

    //clear
    c.clear();
    EXPECT_EQ(c.size(),0);
}

TEST(ListInsertTest, HandleNoneZeroInput)
{
    list c;

    //insert v
    c.insert(c.begin(),-1);
    c.push_back(1);
    c.insert(++c.begin(),0);
    c.push_back(3);
    auto iter = c.begin();
    ++iter;
    ++iter;
    ++iter;
    EXPECT_EQ(*iter,3);
    c.insert(iter,2);
    c.insert(c.end(),4);
    EXPECT_EQ(c.size(),6);
    int i = -1;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}

TEST(ListInsertNTest, HandleNoneZeroInput)
{
    list c;
    std::size_t n = 3;
    int v = 6;
    c.insert_n(c.begin(),n,v);

    EXPECT_EQ(c.size(),n);
    for(int v1:c)
    {
        EXPECT_EQ(v,v1);
    }

}
TEST(ListInsertRangeTest, HandleNoneZeroInput)
{
    list c;

    list flag;
    flag.push_back(100);
    flag.push_back(101);
    flag.push_back(102);


    c.insert(c.begin(),flag.begin(),flag.end());
    c.push_front(0);
    c.push_back(1);
    c.push_back(2);

    auto iter = c.begin();//0
    ++iter;//100
    ++iter;//101
    ++iter;//102
    ++iter;//1
    ++iter;//2

    c.insert(iter,flag.begin(),flag.end());
    c.insert(c.end(),flag.begin(),flag.end());

    EXPECT_EQ(c.size(),4 * 3);
    int i = 0;
    for(int& v:c)
    {
        int m = i % 4;
        if(m == 0)
        {
            EXPECT_EQ(v,i/4);
        }
        else
        {
            EXPECT_EQ(v,100 + m -1);
        }
        ++i;

    }

}

TEST(ListIteratorTest, HandleNoneZeroInput)
{
    list c;
    for(int i=0;i<10;++i)
    {
        c.push_back(i);
    }
    {
        list::iterator i = c.begin();
        list::reverse_iterator r_i(i);
        EXPECT_EQ(r_i,c.rend());
    }
    {
        list::iterator i = c.end();
        list::reverse_iterator r_i(i);
        EXPECT_EQ(r_i,c.rbegin());
    }
    {
        list::iterator i = c.begin();
        ++i;
        list::reverse_iterator r_i(i);
        EXPECT_EQ(*r_i,*i);
    }

    {
        list::iterator i = c.begin();
        list::const_reverse_iterator r_i(i);
        EXPECT_EQ(r_i,c.rend());
    }
    {
        list::iterator i = c.end();
        list::const_reverse_iterator r_i(i);
        EXPECT_EQ(r_i,c.rbegin());
    }
    {
        list::iterator i = c.begin();
        ++i;
        list::const_reverse_iterator r_i(i);
        EXPECT_EQ(*r_i,*i);
    }



    {
        list::reverse_iterator r_i = c.rbegin();
        list::iterator i = r_i.base();
        EXPECT_EQ(i,c.end());
    }
    {
        list::reverse_iterator r_i = c.rend();
        list::iterator i = r_i.base();
        EXPECT_EQ(i,c.begin());
    }
    {
        list::reverse_iterator r_i = c.rbegin();
        ++r_i;
        list::iterator i = r_i.base();
        EXPECT_EQ(*r_i,*i);
    }

    {
        list::reverse_iterator r_i = c.rbegin();
        list::const_iterator i = r_i.base();
        EXPECT_EQ(i,c.end());
    }
    {
        list::reverse_iterator r_i = c.rend();
        list::const_iterator i = r_i.base();
        EXPECT_EQ(i,c.begin());
    }
    {
        list::reverse_iterator r_i = c.rbegin();
        ++r_i;
        list::const_iterator i = r_i.base();
        EXPECT_EQ(*r_i,*i);
    }

}

TEST(ListSwapTest, HandleNoneZeroInput)
{
    list c;

    list left,right;
    for(int i=0;i<10;++i)
    {
        left.push_back(i);
        right.push_back(i);
    }
    right.reverse();
    int i = 9;
    for(int& v:right)
    {
        EXPECT_EQ(v,i);
        --i;
    }

    left.swap(right);
    EXPECT_EQ(left.size(),10);
    EXPECT_EQ(left.size(),10);

    i = 9;
    for(int& v:left)
    {
        EXPECT_EQ(v,i);
        --i;
    }

    i = 0;
    for(int& v:right)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}

TEST(ListEraseTest, HandleNoneZeroInput)
{
    list c;

    c.push_back(-1);
    c.push_back(0);
    c.push_back(-2);
    c.push_back(1);
    c.push_back(-3);

    c.erase(c.begin());
    c.erase(++c.begin());
    c.erase(--c.end());

    int i = 0;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}
TEST(ListEraseRangeTest, HandleNoneZeroInput)
{
    list c;

    list flag;
    flag.push_back(100);
    flag.push_back(101);
    flag.push_back(102);


    c.insert(c.end(),flag.begin(),flag.end());
    list::iterator b0 = c.begin();
    c.push_back(0);
    list::iterator e0 = --c.end();

    list::iterator b1 = e0;
    c.insert(c.end(),flag.begin(),flag.end());
    c.push_back(1);
    list::iterator e1 = --c.end();
    ++b1;

    list::iterator b2 = e1;
    c.insert(c.end(),flag.begin(),flag.end());
    list::iterator e2 = c.end();
    ++b2;


    c.erase(b0,e0);
    c.erase(b1,e1);
    c.erase(b2,e2);
    //show_container(c);

    int i = 0;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}

TEST(ListRemoveTest, HandleNoneZeroInput)
{
    list c;

    c.push_back(99);
    c.push_back(0);
    c.push_back(99);
    c.push_back(1);
    c.push_back(99);

    c.remove(99);
    EXPECT_EQ(c.size(),2);

    int i = 0;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}
TEST(ListRemoveIfTest, HandleNoneZeroInput)
{
    list c;

    c.push_back(99);
    c.push_back(0);
    c.push_back(98);
    c.push_back(1);
    c.push_back(97);

    c.remove_if([](const int v){
        return v > 10;
    });
    EXPECT_EQ(c.size(),2);

    int i = 0;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}

TEST(ListSortTest, HandleNoneZeroInput)
{
    list c;

    c.push_back(2);
    c.push_back(4);
    c.push_back(3);
    c.push_back(1);
    c.push_back(0);

    c.sort();
    int i = 0;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        ++i;
    }

    c.sort([](const int l,const int r){
         return l > r;
    });

    i = 4;
    for(int& v:c)
    {
        EXPECT_EQ(v,i);
        --i;
    }
}

TEST(ListMergeTest, HandleNoneZeroInput)
{
    {
        list c1,c2;

        c1.push_back(2);
        c1.push_back(4);
        c2.push_back(3);
        c2.push_back(1);
        c2.push_back(0);

        c1.merge(c2);
        EXPECT_EQ(c1.size(),5);
        EXPECT_TRUE(c2.empty());
        int i = 0;
        for(int& v:c1)
        {
            EXPECT_EQ(v,i);
            ++i;
        }
    }

    {
        list c1,c2;

        c1.push_back(2);
        c1.push_back(4);
        c2.push_back(3);
        c2.push_back(1);
        c2.push_back(0);

        c1.merge(c2,[](const int l,const int r){
             return l > r;
        });
        EXPECT_EQ(c1.size(),5);
        EXPECT_TRUE(c2.empty());
        int i = 4;
        for(int& v:c1)
        {
            EXPECT_EQ(v,i);
            --i;
        }
    }
}
TEST(ListCopyTest, HandleNoneZeroInput)
{
    int n = 10;
    list c1,c2;
    for(int i=0;i<n;++i)
    {
        c1.push_back(i);
    }
    c2.push_back(666);
    c2 = c1;
    EXPECT_EQ(c2.size(),n);
    int i = 0;
    for(int& v:c2)
    {
        EXPECT_EQ(v,i);
        ++i;
    }

    list c3(c1);
    EXPECT_EQ(c3.size(),n);
    i = 0;
    for(int& v:c3)
    {
        EXPECT_EQ(v,i);
        ++i;
    }
}
