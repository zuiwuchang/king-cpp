#include <gtest/gtest.h>
#include <king/strings/Iconv.hpp>

int main(int argc, char* argv[])
{
    testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

TEST(IconvTest, HandleNoneZeroInput)
{
    //utf8 to gbk
    king::strings::Iconv it_gbk(KING_STRINGS_CODE_GBK,KING_STRINGS_CODE_UTF8);
    //gbk to utf8
    king::strings::Iconv it_utf8(KING_STRINGS_CODE_UTF8,KING_STRINGS_CODE_GBK);

    std::string u1 = u8"this is utf8 轉 gbk 測試 123\n";
    std::string g1 = it_gbk.Translate(u1);
    std::string c1 = it_utf8.Translate(g1);
    EXPECT_EQ(u1,c1);

    u1 = u8"456 this is utf8 轉 gbk 測試\n";
    g1 = it_gbk.Translate(u1);
    c1 = it_utf8.Translate(g1);
    EXPECT_EQ(u1,c1);


}
