#include "RLEFile.h"
#include "RleData.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>
#include "FileSystem.h"

void RleFile::CreateArchive(const std::string& source)
{
    //Provided code from assignment file - opens file in binary mode and stores it in memblock array
    std::ifstream::pos_type size;
    char* memblock;
    
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg(); // Save the size of the file
        memblock = new char [static_cast<unsigned int>(size)];
        file.seekg (0, std::ios::beg); // Seek back to start of file
        file.read (memblock, size);
        file.close();
        
        mData.Compress(memblock, static_cast<unsigned int>(size));
            
        std::string newSource = source + ".rl1";
        
        //Write out the compressed file
        std::ofstream arc(newSource, std::ios::out|std::ios::binary|std::ios::trunc);
        if (arc.is_open())
        {
            mHeader.sig[0] = 'R';
            mHeader.sig[1] = 'L';
            mHeader.sig[2] = 'E';
            mHeader.sig[3] = '\x01';
            mHeader.fileSize = static_cast<unsigned int>(size);
            mHeader.fileName = source;
            mHeader.fileNameLength = static_cast<unsigned char>(source.length());

            arc.write(mHeader.sig, 4);
            arc.write(reinterpret_cast<char*>(&(mHeader.fileSize)), 4);
            arc.write(reinterpret_cast<char*>(&(mHeader.fileNameLength)), 1);
            arc.write((mHeader.fileName).c_str(), mHeader.fileNameLength);
            arc.write(mData.mData, mData.mSize);
        }
        
        // Make sure to clean up!
        delete[] memblock;
    }

}

void RleFile::ExtractArchive(const std::string& source)
{
	///Provided code from assignment file - opens file in binary mode and stores it in memblock array
    std::ifstream::pos_type size;
    char* memblock;
    
    // Open the file for input, in binary mode, and start ATE (at the end)
    std::ifstream file (source, std::ios::in|std::ios::binary|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg(); // Save the size of the file
        memblock = new char [static_cast<unsigned int>(size)];
        file.seekg (0, std::ios::beg); // Seek back to start of file
        file.read (memblock, size);
        file.close();
        
        //Check file signature
        if (size > 4)
        {
            if (memblock[0] != 'R')
            {
                std::cout<<"Error with file signature. Ending decompression.\n";
                return;
            }
            if (memblock[1] != 'L')
            {
                std::cout<<"Error with file signature. Ending decompression.\n";
                return;
            }
            if (memblock[2] != 'E')
            {
                std::cout<<"Error with file signature. Ending decompression.\n";
                return;
            }
            if (memblock[3] != '\x01')
            {
                std::cout<<"Error with file signature. Ending decompression.\n";
                return;
            }
        }
        else
        {
            std::cout<<"Missing file signature. Ending decompression.\n";
            return;
        }
        
        //Load mHeader
        for (int i=0; i<4; i++)
        {
            mHeader.sig[i] = memblock[i];
        }
        
        mHeader.fileSize = *(reinterpret_cast<int*>(&memblock[4]));
        mHeader.fileNameLength = memblock[8];
        
        int fileLengthConverted = static_cast<int>(static_cast<unsigned>(mHeader.fileNameLength));
        int fileDifference = 9+fileLengthConverted;
        for (int i=0; i<fileLengthConverted; i++)
        {
            mHeader.fileName = mHeader.fileName + memblock[9+i];
        }
        
        //Create buffer with only original file content
        int newSize = static_cast<unsigned int>(size) - fileDifference;
        char* subMemblock = new char[newSize];
        memcpy(subMemblock, &memblock[fileDifference], newSize);
        
        mData.Decompress(subMemblock, newSize, mHeader.fileSize);
        
        //Write out the compressed file
        std::ofstream arc(mHeader.fileName, std::ios::out|std::ios::binary|std::ios::trunc);
        if (arc.is_open())
        {
            arc.write(mData.mData, mHeader.fileSize);
        }
        
        // Make sure to clean up!
        delete[] memblock;
        delete[] subMemblock;
    }

}
