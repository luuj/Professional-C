#include "RleData.h"
#include <iostream>
#include <math.h>

void RleData::Compress(const char* input, size_t inSize)
{
    //Reset struct variables
    delete[] mData;
    mSize = 0;
    mData = new char[inSize*2];
    
    //RLE algorithm state variables
    int runState = NEGATIVE;
    char currentChar = NULL;
    char nextChar = NULL;
    int currentRunSize = 1;
    int negativePlaceholder = -1;
    
    //Check for empty input and size one inputs
    if (inSize == 0)
    {
        return;
    }
    if (inSize == 1)
    {
        mData[0] = (char)1;
        mData[1] = input[0];
        mSize = 2;
        return;
    }
    
    //RLE Compress Algorithm
    for (int i=0; i<inSize-1; i++)
    {
        currentChar = input[i];
        nextChar = input[i+1];
        
        if (currentChar == nextChar)
        {
            if (runState == NEGATIVE)
            {
                runState = POSITIVE;
                
                //Insert data into output & reset counter at end of negative run
                if (negativePlaceholder != -1)
                {
                    currentRunSize--;
                    mData[negativePlaceholder] = -currentRunSize;
                    negativePlaceholder = -1;
                    currentRunSize = 1;
                }
            }
            
            //Check for positive run sizes larger than 127
            if (currentRunSize == 127){
                mData[mSize] = currentRunSize;
                mSize++;
                mData[mSize] = currentChar;
                mSize++;
                currentRunSize = 0;
            }
            
            currentRunSize++;
        }
        else
        {
            //Insert data into output & reset counter at end of positive run
            if (runState == POSITIVE)
            {
                mData[mSize] = currentRunSize;
                mSize++;
                mData[mSize] = currentChar;
                mSize++;
                
                currentRunSize = 1;
                runState = NEGATIVE;
            }
            //Save location of counter in mData and add running characters for a negative run
            else
            {
                if (negativePlaceholder == -1)
                {
                    negativePlaceholder = mSize;
                    mSize++;
                }
                
                mData[mSize] = currentChar;
                mSize++;
                
                //Check for negative run sizes larger than 127
                if (currentRunSize == 127)
                {
                    mData[negativePlaceholder] = -currentRunSize;
                    currentRunSize = 0;
                    negativePlaceholder = -1;
                }
                
                currentRunSize++;
            }
        }
        
        //Final positive data insertion when run ends
        if (i == inSize-2 && runState == POSITIVE)
        {
            mData[mSize] = currentRunSize;
            mSize++;
            mData[mSize] = currentChar;
            mSize++;
        }
        
        //Final negative data insertion when run ends
        if (i == inSize-2 && runState == NEGATIVE)
        {
            //Long positive short positive edge case
            if (currentRunSize == 1)
            {
                mData[mSize] = currentRunSize;
                mSize++;
                mData[mSize] = nextChar;
                mSize++;
            }
            else
            {
                mData[mSize] = nextChar;
                mSize++;
                mData[negativePlaceholder] = -currentRunSize;
            }
        }
    }
    
    //File compression percentage
    double initialSize = static_cast<double>(inSize);
    double compressionPercent = ((initialSize - (double)mSize) / initialSize) * 100;
    
    std::cout<< "File compressed from " << initialSize << " bytes to " << mSize << " bytes. " <<  compressionPercent << "% compression." <<std::endl;
    
}

void RleData::Decompress(const char* input, size_t inSize, size_t outSize)
{
    //Reset struct variables
    delete[] mData;
    mSize = 0;
    mData = new char[outSize];

    //Check for empty output
    if (outSize == 0)
    {
        return;
    }
    
    int currNumberInput;
    int currPointer = 0;

    //RLE Decompress Algorithm
    while (currPointer <= inSize)
    {
        currNumberInput = input[currPointer];
        currPointer++;
        
        if (currNumberInput > 0)
        {
            //Positive run decompression
            for (int i=0; i<currNumberInput; i++)
            {
                mData[mSize] = input[currPointer];
                mSize++;
            }
            currPointer++;
        }
        else
        {
            //Negative run decompression
            for (int i=0; i<(-currNumberInput); i++)
            {
                mData[mSize] = input[currPointer];
                mSize++;
                currPointer++;
            }
        }

    }
}

std::ostream& operator<< (std::ostream& stream, const RleData& rhs)
{
	for (int i = 0; i < rhs.mSize; i++)
	{
		stream << rhs.mData[i];
	}
	return stream;
}