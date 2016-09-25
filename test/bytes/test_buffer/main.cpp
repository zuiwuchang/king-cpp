#include <cstdio>
#include <gtest/gtest.h>

#include <king/bytes/type.hpp>




int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
TEST(TypeTest, HandleNoneZeroInput)
{
    int size = 12;
    king::bytes::bytes_t<> bs0(size);
    EXPECT_TRUE(bs0);

    king::bytes::bytes_t<> bs1 = std::move(bs0);
    EXPECT_FALSE(bs0);
    EXPECT_TRUE(bs1);

    EXPECT_EQ(bs1.size(),size);

    bs1.reset();
    EXPECT_FALSE(bs1);
}
TEST(TypeFragmentation, HandleNoneZeroInput)
{
    //write
    int size = 5;
    king::bytes::fragmentation_t<> f0(size);
    king::byte_t bytes[size]{1,2,3,4,5};
    EXPECT_EQ(f0.write(bytes,size),size);
    EXPECT_EQ(f0.write(bytes,size),0);

    EXPECT_EQ(f0.get_free(),0);
    EXPECT_EQ(f0.size(),size);
    EXPECT_EQ(f0.capacity(),size);


    //copy to
    king::byte_t copy[size-1];
    EXPECT_EQ(f0.copy_to(copy,size-1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i],copy[i]);
    }

    //copy to skip
    EXPECT_EQ(f0.copy_to(1,copy,size-1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i+1],copy[i]);
    }

    //read
    EXPECT_EQ(f0.read(copy,size -1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i],copy[i]);
    }
    EXPECT_EQ(f0.get_free(),0);
    EXPECT_EQ(f0.size(),1);
    EXPECT_EQ(f0.capacity(),size);

    EXPECT_EQ(f0.read(copy,size - 1),1);
    EXPECT_EQ(copy[0],bytes[size-1]);

    EXPECT_EQ(f0.get_free(),0);
    EXPECT_EQ(f0.size(),0);
    EXPECT_EQ(f0.capacity(),size);
    EXPECT_EQ(f0.read(copy,size - 1),0);

    f0.init();
    EXPECT_EQ(f0.get_free(),size);
    EXPECT_EQ(f0.size(),0);
    EXPECT_EQ(f0.capacity(),size);

    f0.reset();
    EXPECT_EQ(f0.get_free(),0);
    EXPECT_EQ(f0.size(),0);
    EXPECT_EQ(f0.capacity(),0);
}
