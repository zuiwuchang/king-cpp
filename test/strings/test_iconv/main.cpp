#include <gtest/gtest.h>
#include <king/strings/iconv.hpp>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(IconvTest, HandleNoneZeroInput)
{
    //utf8 to gbk
    king::strings::iconv_t it_gbk(KING_STRINGS_CODE_GBK,KING_STRINGS_CODE_UTF8);
    //gbk to utf8
    king::strings::iconv_t it_utf8(KING_STRINGS_CODE_UTF8,KING_STRINGS_CODE_GBK);

    std::string u1 = u8"this is utf8 轉 gbk 測試 123\n";
    std::string g1 = it_gbk.iconv(u1);
    std::string c1 = it_utf8.iconv(g1);
    EXPECT_EQ(u1,c1);

    u1 = u8"456 this is utf8 轉 gbk 測試\n";
    g1 = it_gbk.iconv(u1);
    c1 = it_utf8.iconv(g1);
    EXPECT_EQ(u1,c1);


}
