#pragma once
#include <ostream>
#include <iostream>

// RLE Compression/Decompression
struct RleData
{
	// Memory which stores either compressed or decompressed data
	char* mData;
	// Number of elements of type T that data is pointing to
	int mSize;
    const int NEGATIVE = -1;
    const int UNKNOWN = 0;
    const int POSITIVE = 1;
    
	RleData()
	: mData(nullptr)
	, mSize(0)
	{ }

	~RleData()
	{
		delete[] mData;
	}
	
	// Compresses input data and stores it in mData
	void Compress(const char* input, size_t inSize);

	// Decompresses input data and stores it in mData
	void Decompress(const char* input, size_t inSize, size_t outSize);

	// Outputs mData
	friend std::ostream& operator<< (std::ostream& stream, const RleData& rhs);
	
	size_t MaxRunSize()
	{
		// Bitwise magic
		return 127;
	}
};

