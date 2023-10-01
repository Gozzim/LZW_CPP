#include "../src/LZW.h"
#include <chrono>
#include <gtest/gtest.h>
#include <iostream>
#include <random>

TEST(LZWTest, CompressionAndDecompression)
{
    unsigned int seed = static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count());
    std::mt19937 rng(seed);
    std::uniform_int_distribution<int> distribution(0, 255);
    std::string input1;
    std::string input2;
    std::string input3;
    std::string input4;

    for (int i = 1; i <= 1000000; ++i) {
        input1 += static_cast<char>(distribution(rng));
        input2 += static_cast<char>(distribution(rng) % 128);
        input3 += static_cast<char>(i % 256);
        input4 += static_cast<char>(100);
    }

    auto [compressResult1, compressCode1] = sLZW->Compress(input1);
    ASSERT_EQ(compressCode1, LZW_OK);
    auto [compressResult2, compressCode2] = sLZW->Compress(input2);
    ASSERT_EQ(compressCode2, LZW_OK);
    auto [compressResult3, compressCode3] = sLZW->Compress(input3);
    ASSERT_EQ(compressCode3, LZW_OK);
    auto [compressResult4, compressCode4] = sLZW->Compress(input4);
    ASSERT_EQ(compressCode4, LZW_OK);

    auto [decompressResult1, decompressCode1] = sLZW->Decompress(compressResult1);
    ASSERT_EQ(decompressCode1, LZW_OK);
    auto [decompressResult2, decompressCode2] = sLZW->Decompress(compressResult2);
    ASSERT_EQ(decompressCode2, LZW_OK);
    auto [decompressResult3, decompressCode3] = sLZW->Decompress(compressResult3);
    ASSERT_EQ(decompressCode3, LZW_OK);
    auto [decompressResult4, decompressCode4] = sLZW->Decompress(compressResult4);
    ASSERT_EQ(decompressCode4, LZW_OK);

    ASSERT_EQ(decompressResult1, input1);
    ASSERT_EQ(decompressResult2, input2);
    ASSERT_EQ(decompressResult3, input3);
    ASSERT_EQ(decompressResult4, input4);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
