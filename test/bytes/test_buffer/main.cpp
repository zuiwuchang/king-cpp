#include <cstdio>
#include <gtest/gtest.h>

#include <king/bytes/Type.hpp>
#include <king/bytes/Buffer.hpp>



int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
TEST(TypeTest, HandleNoneZeroInput)
{
    int size = 12;
    king::bytes::Bytes bs0(size);
    EXPECT_TRUE(bs0);

    king::bytes::Bytes bs1 = std::move(bs0);
    EXPECT_FALSE(bs0);
    EXPECT_TRUE(bs1);

    EXPECT_EQ(bs1.Size(),size);

    bs1.Reset();
    EXPECT_FALSE(bs1);
}
TEST(TypeFragmentation, HandleNoneZeroInput)
{
    //write
    int size = 5;
    king::bytes::Fragmentation f0(size);
    king::Byte bytes[size]{1,2,3,4,5};
    EXPECT_EQ(f0.Write(bytes,size),size);
    EXPECT_EQ(f0.Write(bytes,size),0);

    EXPECT_EQ(f0.GetFree(),0);
    EXPECT_EQ(f0.Size(),size);
    EXPECT_EQ(f0.Capacity(),size);


    //copy to
    king::Byte copy[size-1];
    EXPECT_EQ(f0.CopyTo(copy,size-1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i],copy[i]);
    }

    //copy to skip
    EXPECT_EQ(f0.CopyTo(1,copy,size-1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i+1],copy[i]);
    }

    //read
    EXPECT_EQ(f0.Read(copy,size -1),size-1);
    for(int i=0;i<size - 1;++i)
    {
        EXPECT_EQ(bytes[i],copy[i]);
    }
    EXPECT_EQ(f0.GetFree(),0);
    EXPECT_EQ(f0.Size(),1);
    EXPECT_EQ(f0.Capacity(),size);

    EXPECT_EQ(f0.Read(copy,size - 1),1);
    EXPECT_EQ(copy[0],bytes[size-1]);

    EXPECT_EQ(f0.GetFree(),0);
    EXPECT_EQ(f0.Size(),0);
    EXPECT_EQ(f0.Capacity(),size);
    EXPECT_EQ(f0.Read(copy,size - 1),0);

    f0.Init();
    EXPECT_EQ(f0.GetFree(),size);
    EXPECT_EQ(f0.Size(),0);
    EXPECT_EQ(f0.Capacity(),size);

    f0.Reset();
    EXPECT_EQ(f0.GetFree(),0);
    EXPECT_EQ(f0.Size(),0);
    EXPECT_EQ(f0.Capacity(),0);
}

TEST(TypeBuffer, HandleNoneZeroInput)
{
    {
        //實例化 對象
        king::bytes::Buffer buf(8);
        std::string str = "0123456789";

        //寫入數據
		EXPECT_EQ(buf.Write((const std::uint8_t*)str.data(),str.size()),str.size());
		//獲取 可讀數據 大小
		EXPECT_EQ(buf.Size(),str.size());

		char bytes[11] = {0};
		//copy 可讀 緩衝區
		EXPECT_EQ(buf.CopyTo((std::uint8_t*)bytes,10),str.size());
		EXPECT_STREQ(bytes,str.c_str());

		memset(bytes,0,sizeof(bytes));
		EXPECT_EQ(buf.CopyTo(1,(std::uint8_t*)bytes,10),str.size()-1);
		EXPECT_STREQ(bytes,str.c_str()+1);


		memset(bytes,0,sizeof(bytes));
		int offset = 0;
		int len = 10;
		while (true)
		{
			int need = 3;

			//讀取 緩衝區
			int n = buf.Read((std::uint8_t*)bytes + offset, need);
			if (!n)
			{
				//無 數據 可讀
				EXPECT_STREQ(bytes,str.c_str());
				break;
			}
			offset += n;
			len -= n;
		}

		EXPECT_EQ(buf.Size(),0);
    }

    {
        king::bytes::Buffer buf(8);
		std::string str = "0123456789abcdefghijklmnopqrstwxz";
		EXPECT_EQ(buf.Write((std::uint8_t*)str.data(),str.size()),str.size());

		std::size_t size = str.size();
		char* b = new char[size];
		std::size_t n = buf.CopyTo((std::uint8_t*)b,size);
		EXPECT_EQ(std::string(b,n),str);

		std::size_t pos = 3;
		//跳過字節 copy
		n = buf.CopyTo(pos,(std::uint8_t*)b,size);
		EXPECT_EQ(std::string(b,n),str.substr(pos));
    }
}
