//異常定義
#ifndef KING_LIB_HEADER_CRYPTO_CLASSICAL
#define KING_LIB_HEADER_CRYPTO_CLASSICAL

#include <king/Core.hpp>
#include <ctime>

namespace king
{
namespace crypto
{

/**
*   \brief 古典加密算法
*
*   Classical 作爲一個 namespace 無法被實例化\n
*   其 static 方法 實現類一些 有用的古典加密算法
*/
class Classical
{
private:
    /**
    *   避免被實例化
    */
    Classical(){}
};

/**
*   \brief K3XsxSalt 加密算法
*
*   K3XsxSalt 是孤設計的一個古典加密算法 用於 快速 將數據 編碼爲 不太容易 被解開的 密文\n
*   Salt 顯示了 隨機 撒鹽 以使相同數據 隨機 編碼爲不同的 數據 從而 避免一些 差量分析的逆向 行爲\n
*   3Xsx 顯示了 編碼 將 通過 三步 運算\n
*   1	產生 3個 隨機的 鹽 salts=[0,1,2] 並將 鹽值+一個固定值 Salt\n
*   2	對 待加密的每個 字節 進行 Xsx 三步運算\n
*   3	將 步驟1產生的鹽 salts 寫入輸出數據頭部 將加密後的數據 寫到 鹽後\n\n
*   Xsx\n
*   	//step 0\n
*   	b = b xor (salts[0] + Salt) //異或\n
*   	//step 1\n
*   	swapbit(b,0,7)	//交換 字節 第0bit 與 第7bit 的 值\n
*   	swapbit(b,1,6)\n
*   	swapbit(b,2,5)\n
*   	swapbit(b,3,4)\n
*   	b = b shl-loop (salts[1] + Salt) // shr-loop 左移 但 越位的數據 不捨棄 而是作爲最低位的 補位數據\n
*   	//step 2\n
*   	b = b xor (salts[2] + Salt)\n
*/
class K3XsxSalt
{
protected:
    /**
    *   \brief  鹽基值
    */
    king::Byte _salt;
public:
    /**
    *   \brief  構造
    *   \param salt 鹽基值
    */
    K3XsxSalt(const king::Byte& salt=0)
        :_salt(salt)
    {
    }

    /**
    *   \brief  返回 鹽 長度
    */
    static std::size_t SaltLen()
    {
        return 3;
    }

    /**
    *   \brief  加密數據
    *
    *   加密後的數據 會比原數據 長3字節(存儲隨機產生的 鹽)
    *
    *   \param src     待加密的原數據
    *   \param n       src長度
    *   \param dist    加密後的輸出數據 長度必須 >= n+3
    */
    void Encryption(const king::Byte* src,std::size_t n,king::Byte* dist)
    {
        //創建隨機鹽
        randSalts(dist);

        //對每個字節 進行 加密
        std::size_t saltLen = SaltLen();
        for(std::size_t i = 0; i < n; ++i)
        {
           dist[saltLen+i] = encryption(src[i],dist);
        }
    }
    /**
    *   \brief  解密數據
    *
    *   解密後的數據 會比原數據 短3字節(去掉 鹽)
    *
    *   \param src     待解密的原數據
    *   \param n       src長度    n必須 >=3
    *   \param dist    解密後的輸出數據
    */
    bool Decryption(const king::Byte* src,std::size_t n,king::Byte* dist)
    {
        const std::size_t saltLen = SaltLen();
        if(n < saltLen)
        {
            return false;
        }
        else if(n == saltLen)
        {
            return true;
        }

        //對每個字節 進行 解密
        for(std::size_t i = 0; i < n - saltLen;++i)
        {
                dist[i] = decryption(src[i+saltLen], src);
        }

        return true;
    }
protected:
    /**
    *   \brief  創建隨機的鹽
    *
    *   \param salts    存儲產生的鹽
    */
    void randSalts(king::Byte* salts)
    {
        srand(time(NULL));

        std::size_t n = SaltLen();
        for(std::size_t i=0;i<n;++i)
        {
            unsigned int x = (unsigned int)rand();
            salts[i] = (king::Byte)(x%255);
        }
    }
    /**
    *   \brief  逐字節加密
    *
    *   \param b        等加密 字節
    *   \param salts    3個隨機 鹽
    *   \return         加密後的字節
    */
    king::Byte encryption(const king::Byte b,const king::Byte* salts)
    {

        //step 0
        king::Byte c = b ^ (salts[0] + _salt);

        //step 1
        c = swapBit(c,0,7);
        c = swapBit(c,1,6);
        c = swapBit(c,2,5);
        c = swapBit(c,3,4);
        c = shlLoop(c,salts[1] + _salt);

        //step 2
        c ^= salts[2] + _salt;

        return c;
    }

    /**
    *   \brief  逐字節解密
    *
    *   \param b        等解密 字節
    *   \param salts    3個隨機 鹽
    *   \return         解密後的字節
    */
    king::Byte decryption(const king::Byte b,const king::Byte* salts)
    {
       //step 2
        king::Byte c = b ^ (salts[2] + _salt);

        //step 1
        c = shrLoop(c,salts[1] + _salt);
        c = swapBit(c,0,7);
        c = swapBit(c,1,6);
        c = swapBit(c,2,5);
        c = swapBit(c,3,4);

        //step 0
        c ^= (salts[0] + _salt);
        return c;
    }

private:
    king::Byte swapBit(const king::Byte b,std::size_t l,std::size_t r)
    {
        if(l==r)
        {
            return b;
        }

        king::Byte c = b;
        if(isBit(b,l))
        {
            c = setBit(c,r,true);
        }
        else
        {
            c = setBit(c,r,false);
        }

        if(isBit(b,r))
        {
            c = setBit(c,l,true);
        }
        else
        {
            c = setBit(c,l,false);
        }

        return c;
    }
    bool isBit(const king::Byte b,std::size_t n)
    {
        king::Byte c = 1;
        c<<=n;
        return c & b;
    }
    king::Byte setBit(const king::Byte b,std::size_t n,bool ok)
    {
        if(ok)
        {
            king::Byte c = 1;
            c<<=n;
            return b | c;
        }

        king::Byte c = 1;
        c<<=n;
        return b & (~c);
    }
    king::Byte shlLoop(const king::Byte b,std::size_t n)
    {
        n %=8;
        if(n == 0)
        {
            return b;
        }

        king::Byte c = b << n;
        c |= b >> (8-n);

        return c;
    }
    king::Byte shrLoop(const king::Byte b,std::size_t n)
    {
        n %=8;
        if(n == 0)
        {
            return b;
        }

        king::Byte c = b >> n;
        c |= b << (8-n);

        return c;
    }
};


};
};

#endif // KING_LIB_HEADER_CRYPTO_CLASSICAL
