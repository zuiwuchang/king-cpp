#include <cstdio>
#include <gtest/gtest.h>

#include <king/bytes/type.hpp>
#include <king/bytes/buffer.hpp>



int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
TEST(TypeTest, HandleNoneZeroInput)
{
    int size = 12;
    king::bytes::bytes_t bs0(size);
    EXPECT_TRUE(bs0);

    king::bytes::bytes_t bs1 = std::move(bs0);
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
    king::bytes::fragmentation_t f0(size);
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

TEST(TypeBuffer, HandleNoneZeroInput)
{
    {
        //實例化 對象
        king::bytes::buffer_t buf(8);
        std::string str = "0123456789";

        //寫入數據
		EXPECT_EQ(buf.write((const std::uint8_t*)str.data(),str.size()),str.size());
		//獲取 可讀數據 大小
		EXPECT_EQ(buf.size(),str.size());

		char bytes[11] = {0};
		//copy 可讀 緩衝區
		EXPECT_EQ(buf.copy_to((std::uint8_t*)bytes,10),str.size());
		EXPECT_STREQ(bytes,str.c_str());

		memset(bytes,0,sizeof(bytes));
		EXPECT_EQ(buf.copy_to(1,(std::uint8_t*)bytes,10),str.size()-1);
		EXPECT_STREQ(bytes,str.c_str()+1);


		memset(bytes,0,sizeof(bytes));
		int offset = 0;
		int len = 10;
		while (true)
		{
			int need = 3;

			//讀取 緩衝區
			int n = buf.read((std::uint8_t*)bytes + offset, need);
			if (!n)
			{
				//無 數據 可讀
				EXPECT_STREQ(bytes,str.c_str());
				break;
			}
			offset += n;
			len -= n;
		}

		EXPECT_EQ(buf.size(),0);
    }

    {
        king::bytes::buffer_t buf(8);
		std::string str = "0123456789abcdefghijklmnopqrstwxz";
		EXPECT_EQ(buf.write((std::uint8_t*)str.data(),str.size()),str.size());

		std::size_t size = str.size();
		char* b = new char[size];
		std::size_t n = buf.copy_to((std::uint8_t*)b,size);
		EXPECT_EQ(std::string(b,n),str);

		std::size_t pos = 3;
		//跳過字節 copy
		n = buf.copy_to(pos,(std::uint8_t*)b,size);
		EXPECT_EQ(std::string(b,n),str.substr(pos));
    }
}
