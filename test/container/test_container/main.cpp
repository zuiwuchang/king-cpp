#include <gtest/gtest.h>

#include <king/container/LoopBuffer.hpp>
#include <exception>
int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
#define LOOP_BUFFER_N   5
TEST(LoopBufferIteratorTest, HandleNoneZeroInput)
{
    typedef  king::container::LoopBuffer<std::size_t,LOOP_BUFFER_N> LoopBuffer;
    LoopBuffer loop;
    for(std::size_t i=0; i<loop.Capacity(); ++i)
    {
        loop<<i;
    }

    {
        std::size_t i=0;
        for(auto v:loop)
        {
            EXPECT_EQ(v,i++);
        }
        EXPECT_EQ(LOOP_BUFFER_N,i);

        i = 0;
        LoopBuffer::Iterator iter = loop.Begin();
        for(;iter != loop.End();++iter)
        {
            EXPECT_EQ(*iter,i++);
        }
        EXPECT_EQ(LOOP_BUFFER_N,i);
        --iter;
        for(;iter!=loop.Begin();--iter)
        {
            EXPECT_EQ(*iter,--i);
        }
        EXPECT_EQ(*iter,0);
    }
    {
        std::size_t i=0;
        LoopBuffer::ConstIterator iter = loop.Begin();
        for(;iter != loop.End();++iter)
        {
            EXPECT_EQ(*iter,i++);
        }
    }

}
TEST(LoopBufferTest, HandleNoneZeroInput)
{
    king::container::LoopBuffer<std::size_t,LOOP_BUFFER_N> loop;
    EXPECT_TRUE(loop.Empty());
    EXPECT_TRUE(!loop);
    EXPECT_EQ(loop.Free(),LOOP_BUFFER_N);
    EXPECT_EQ(loop.Size(),0);
    EXPECT_EQ(loop.Capacity(),LOOP_BUFFER_N);
    EXPECT_FALSE(loop.IsRead());
    EXPECT_TRUE(loop.IsWrite());

    for(std::size_t i=0; i<loop.Capacity(); ++i)
    {
        EXPECT_TRUE(loop.TryWrite(i));

        EXPECT_TRUE(!loop.Empty());
        EXPECT_TRUE(loop);
        EXPECT_EQ(loop.Size(),i+1);
        EXPECT_EQ(loop.Free(),LOOP_BUFFER_N-i-1);
        EXPECT_TRUE(loop.IsRead());
    }
    EXPECT_FALSE(loop.IsWrite());
    EXPECT_FALSE(loop.TryWrite(100));

    {
        EXPECT_TRUE(loop.IsRead());
        std::size_t v = 100;
        EXPECT_TRUE(loop.TryRead(v));
        EXPECT_EQ(v,0);

        EXPECT_TRUE(loop.IsWrite());
        EXPECT_EQ(loop.Free(),1);
        EXPECT_EQ(loop.Size(),LOOP_BUFFER_N-1);

        for(std::size_t i = 1; i<LOOP_BUFFER_N; ++i)
        {
            std::size_t v = 100;
            EXPECT_TRUE(loop.TryRead(v));
            EXPECT_EQ(v,i);

            EXPECT_EQ(loop.Size(),LOOP_BUFFER_N - 1- i);
            EXPECT_EQ(loop.Free(),i+1);
        }
        EXPECT_TRUE(loop.Empty());
        EXPECT_TRUE(!loop);
        EXPECT_EQ(loop.Free(),LOOP_BUFFER_N);
        EXPECT_EQ(loop.Size(),0);
        EXPECT_EQ(loop.Capacity(),LOOP_BUFFER_N);
        EXPECT_FALSE(loop.IsRead());
        EXPECT_TRUE(loop.IsWrite());
    }

    {
        for(std::size_t i=0; i<loop.Capacity(); ++i)
        {
            EXPECT_TRUE(loop.TryWrite(i));
        }
        std::size_t v = 100;
        EXPECT_TRUE(loop.TryRead(v));
        EXPECT_EQ(v,0);

        EXPECT_TRUE(loop.TryWrite(LOOP_BUFFER_N));
        for(std::size_t i = 0; i<LOOP_BUFFER_N; ++i)
        {
            std::size_t v = 100;
            EXPECT_TRUE(loop.TryRead(v));
            EXPECT_EQ(v,i+1);
        }
    }

    {
        EXPECT_TRUE(loop.Empty());
        EXPECT_NO_THROW(loop.Write(0).Write(1).Write(2).Write(3).Write(4));
        EXPECT_THROW(loop.Write(5),king::container::BadLoopBufferWrite);

        std::size_t v = 0;
        EXPECT_NO_THROW(loop.Read(v).Read(v).Read(v).Read(v).Read(v));
        EXPECT_EQ(v,4);
        EXPECT_THROW(loop.Read(v),king::container::BadLoopBufferRead);

        EXPECT_TRUE(loop.Empty());
    }

    {
        EXPECT_TRUE(loop.Empty());
        EXPECT_NO_THROW(loop<<0<<1<<2<<3<<4);
        EXPECT_THROW(loop<<5,king::container::BadLoopBufferWrite);

        std::size_t v = 0;
        EXPECT_NO_THROW(loop>>v>>v>>v>>v>>v);
        EXPECT_EQ(v,4);
        EXPECT_THROW(loop.Read(v),king::container::BadLoopBufferRead);

        EXPECT_TRUE(loop.Empty());
    }
}
