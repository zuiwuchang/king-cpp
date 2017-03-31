#include <gtest/gtest.h>
#include <king/crypto/Classical.hpp>
#include <iostream>
#include <string>

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
TEST(K3XsxSaltTest, HandleNoneZeroInput)
{
    king::crypto::K3XsxSalt k3(2);

    char buf[1024] = {0};
    char dec[1024] = {0};

    std::string str = "this is 測試 text";
    std::size_t n = str.size();
    k3.Encryption((const king::Byte*)str.data(),n,(king::Byte*)buf);

    k3.Decryption((const king::Byte*)buf,n+3,(king::Byte*)dec);

    std::string dStr(dec,n);

	EXPECT_EQ(str,dStr);
}
