#pragma once
#include "MiniCppUnit.hpp"
#include "RleData.h"
#include <iostream>
#include <iomanip>

// Helper functions
bool buffersAreSame(const char* expected, const char* actual, size_t size);
void runCompressionTest(char* test, size_t testSize,
	char* expected, size_t expectedSize);
void runDecompressionTest(char* test, size_t testSize,
	char* expected, size_t expectedSize);

// IMPORTANT NOTE --
// The tests all use "sizeof" to find the size of the input
// buffer. However, remember that this only works because the
// character arrays are statically allocated.
// If you just have a pointer to a C-style string, "sizeof"
// will return the size of the pointer type (either 4 or 8).
class CompressionTests : public TestFixture<CompressionTests>
{
public:
	TEST_FIXTURE_DESCRIBE(CompressionTests, "Testing Compression...")
	{
		TEST_CASE_DESCRIBE(testBasicPositiveRuns, "Basic positive run test");
        TEST_CASE_DESCRIBE(testLongPositiveRuns, "Long positive run test");
        TEST_CASE_DESCRIBE(testSingleCharacterRuns, "Single character run test");
        TEST_CASE_DESCRIBE(testLongNegativeRuns, "Long negative run test");
        TEST_CASE_DESCRIBE(testBasicAlternatingRuns, "Basic alternating run starting with positive test");
        TEST_CASE_DESCRIBE(testBasicAlternatingRunsNegative, "Basic alternating run starting with negative test");
        TEST_CASE_DESCRIBE(testExtraLongPositiveRuns, "Extra long (>127) positive run test");
        TEST_CASE_DESCRIBE(testExtraLongNegativeRuns, "Extra long (>127) negative run test");
        TEST_CASE_DESCRIBE(testShortNegativeRuns, "Short negative run test");
        TEST_CASE_DESCRIBE(testLongPositiveShortNegative, "Long positive short negative run test");
        TEST_CASE_DESCRIBE(testLongNegativeShortPositive, "Long negative short positive run test");
        TEST_CASE_DESCRIBE(testLongPositiveShortPositive, "Long positive short positive run test");
        TEST_CASE_DESCRIBE(testAlternatingNegativeRun, "Alternating full negative run test");
        TEST_CASE_DESCRIBE(testExtraLongPositiveShortNegative, "Extra long (>127) positive short negative run test");
        TEST_CASE_DESCRIBE(testExtraLongNegativeShortPositive, "Extra long (>127) negative short positive run test");
	}
	
	void testBasicPositiveRuns()
	{
		char test[] = "aaabbbcccdddaaabbbcccdddaaabbbcccdddaaabbbc"
			"ccdddaaabbbcccdddaaabbbcccdddaaabbbcccdddaaabbbcccddd";
		char expected[] = "\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d"
			"\x03" "a" "\x03" "b" "\x03" "c" "\x03" "d";
		
		runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
	}
    
    void testLongPositiveRuns()
    {
        char test[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbb"
            "cccccccccccccccccccccccccccccc"
            "dddddddddddddddddddddddddddddd";
        char expected[] = "\x1e" "a" "\x1e" "b" "\x1e" "c" "\x1e" "d";
        
		runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testSingleCharacterRuns()
    {
        char test[] = "a";
        char expected[] = "\x01" "a";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongNegativeRuns()
    {
        char test[] = "abcdefghijklmnopqrstuvwxyz";
        char expected[] = "\xE6" "abcdefghijklmnopqrstuvwxyz";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testBasicAlternatingRuns()
    {
        char test[] = "aaaaaaaaaa" "bcdefghijk" "bbbbbbbbbb"
            "lmnopqrstu" "cccccccccc";
        char expected[] = "\x0A" "a" "\xF6" "bcdefghijk"
            "\x0A" "b" "\xF6" "lmnopqrstu" "\x0A" "c";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testBasicAlternatingRunsNegative()
    {
        char test[] = "bcdefghijk" "aaaaaaaaaa" "lmnopqrstu"
            "bbbbbbbbbb" "poitrewqlk";
        char expected[] = "\xF6" "bcdefghijk" "\x0A" "a"
            "\xF6" "lmnopqrstu" "\x0A" "b" "\xF6" "poitrewqlk";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongPositiveRuns()
    {
        char test[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        char expected[] = "\x7F" "a" "\x7F" "a" "\x26" "a";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongNegativeRuns()
    {
        char test[] = "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyzabcdef";
        char expected[] = "\x81" "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw"
            "\x81" "xyzabcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrst"
            "\xDA" "uvwxyz"
            "abcdefghijklmnopqrstuvwxyzabcdef";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testShortNegativeRuns()
    {
        char test[] = "abcde";
        char expected[] = "\xFB" "abcde";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongPositiveShortNegative()
    {
        char test[] = "aaaaaaaaaaaaaaaa" "bc";
        char expected[] = "\x10" "a" "\xFE" "bc";
    
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongNegativeShortPositive()
    {
        char test[] = "abcdefghijklmnop" "aa";
        char expected[] = "\xF0" "abcdefghijklmnop" "\x02" "a";
    
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongPositiveShortPositive()
    {
        char test[] = "aaaaaaaaaaaaaaaa" "b";
        char expected[] = "\x10" "a" "\x01" "b";

        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testAlternatingNegativeRun()
    {
        char test[] = "ababababab";
        char expected[] = "\xF6" "ababababab";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongPositiveShortNegative()
    {
        char test[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "ba";
        char expected[] = "\x7F" "a" "\xFE" "ba";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongNegativeShortPositive()
    {
        char test[] = "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw" "bb";
        char expected[] = "\x81" "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw"
            "\x02" "b";
        
        runCompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
};

class DecompressionTests : public TestFixture<DecompressionTests>
{
public:
	TEST_FIXTURE_DESCRIBE(DecompressionTests, "Testing Decompression...")
	{
		TEST_CASE_DESCRIBE(testBasicPositiveRuns, "Basic positive decompression run test");
        TEST_CASE_DESCRIBE(testSingleCharRuns, "Single character decompression run test");
        TEST_CASE_DESCRIBE(testBasicNegativeRuns, "Basic negative decompression run test");
        TEST_CASE_DESCRIBE(testAlternatingRuns, "Basic alternating positive decompression run test");
        TEST_CASE_DESCRIBE(testAlternatingRunsNegative, "Basic alternating negative decompression run test");
        TEST_CASE_DESCRIBE(testExtraLongPositiveRuns, "Extra long (>127) positive decompression run test");
        TEST_CASE_DESCRIBE(testExtraLongNegativeRuns, "Extra long (>127) negative decompression run test")
        TEST_CASE_DESCRIBE(testLongPositiveShortNegative, "Long positive short negative decompression run test");
        TEST_CASE_DESCRIBE(testLongNegativeShortPositive, "Long negative short positive decompression run test");
        TEST_CASE_DESCRIBE(testLongPositiveShortPositive, "Long positive short positive decompression run test");
        TEST_CASE_DESCRIBE(testAlternatingNegativeRun, "Alternating negative decompression run test");
        TEST_CASE_DESCRIBE(testExtraLongPositiveShortNegative, "Extra long positive short negative decompression run test");
        TEST_CASE_DESCRIBE(testExtraLongNegativeShortPositive, "Extra long negative short positive decompression run test");
	}
	
	void testBasicPositiveRuns()
	{
		char test[] = "\x28" "x";
		char expected[] = "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		
		runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
	}
    
    void testSingleCharRuns()
    {
        char test[] = "\x01" "a";
        char expected[] = "a";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testBasicNegativeRuns()
    {
        char test[] = "\xE6" "abcdefghijklmnopqrstuvwxyz";
        char expected[] = "abcdefghijklmnopqrstuvwxyz";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testAlternatingRuns()
    {
        char test[] = "\x0A" "a" "\xF6" "bcdefghijk"
            "\x0A" "b" "\xF6" "lmnopqrstu" "\x0A" "c";
        char expected[] = "aaaaaaaaaa" "bcdefghijk" "bbbbbbbbbb"
            "lmnopqrstu" "cccccccccc";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testAlternatingRunsNegative()
    {
        char test[] = "\xF6" "bcdefghijk" "\x0A" "a"
            "\xF6" "lmnopqrstu" "\x0A" "b" "\xF6" "poitrewqlk";
        char expected[] = "bcdefghijk" "aaaaaaaaaa" "lmnopqrstu"
            "bbbbbbbbbb" "poitrewqlk";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongPositiveRuns()
    {
        char test[] = "\x7F" "a" "\x7F" "a" "\x26" "a";
        char expected[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongNegativeRuns()
    {
        char test[] = "\x81" "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw"
            "\x81" "xyzabcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrst"
            "\xDA" "uvwxyz"
            "abcdefghijklmnopqrstuvwxyzabcdef";
        
        char expected[] = "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyzabcdef";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongPositiveShortNegative()
    {
        char test[] = "\x10" "a" "\xFE" "bc";
        char expected[] = "aaaaaaaaaaaaaaaa" "bc";
    
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongNegativeShortPositive()
    {
        char test[] = "\xF0" "abcdefghijklmnop" "\x02" "a";
        char expected[] = "abcdefghijklmnop" "aa";
    
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testLongPositiveShortPositive()
    {
        char test[] = "\x10" "a" "\x01" "b";
        char expected[] = "aaaaaaaaaaaaaaaa" "b";

        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testAlternatingNegativeRun()
    {
        char test[] = "\xF6" "ababababab";
        char expected[] = "ababababab";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongPositiveShortNegative()
    {
        char test[] = "\x7F" "a" "\xFE" "ba";
        char expected[] = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
            "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa" "ba";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
    
    void testExtraLongNegativeShortPositive()
    {
        char test[] = "\x81" "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw"
            "\x02" "b";
        char expected[] = "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvwxyz"
            "abcdefghijklmnopqrstuvw" "bb";
        
        runDecompressionTest(test, sizeof(test) - 1, expected, sizeof(expected) - 1);
    }
};

REGISTER_FIXTURE(CompressionTests);
REGISTER_FIXTURE(DecompressionTests);

bool buffersAreSame(const char* expected, const char* actual, size_t size)
{
	if (actual == nullptr)
	{
		std::cerr << std::endl << "Actual data is null." << std::endl;
		return false;
	}

	bool retVal = true;

	for (size_t i = 0; i < size; i++)
	{
		if (expected[i] != actual[i])
		{
			retVal = false;
			std::cerr << std::endl
				<< "Expected 0x" << std::hex << static_cast<unsigned>(expected[i])
				<< " at index " << std::dec << i << " but saw 0x"
				<< std::hex << static_cast<unsigned>(actual[i]) << std::endl;
			break;
		}
	}

	return retVal;
}

void runCompressionTest(char* test, size_t testSize, 
	char* expected, size_t expectedSize)
{
	RleData r;
	r.Compress(test, testSize);
	ASSERT_TEST_MESSAGE(buffersAreSame(expected, r.mData, expectedSize),
		"Buffer mismatch (see above for details)");
}

void runDecompressionTest(char* test, size_t testSize,
	char* expected, size_t expectedSize)
{
	RleData r;
	r.Decompress(test, testSize, expectedSize);
	ASSERT_TEST_MESSAGE(buffersAreSame(expected, r.mData, expectedSize),
		"Buffer mismatch (see above for details)");
}
