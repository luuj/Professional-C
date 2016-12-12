#include "FASTAParser.h"
#include "Exceptions.h"
#include <iostream>
#include <fstream>

FASTAParser::FASTAParser(){}

FASTAParser::FASTAParser(const std::string& filePath)
{
    std::ifstream::pos_type size;
    char currChar;
    char validChar[] = {'A', 'T', 'C', 'G', '\n', '\r', '\xff'};
    
    // Open the file for input and start ATE (at the end)
    std::ifstream file (filePath, std::ios::in|std::ios::ate);
    if (file.is_open())
    {
        size = file.tellg(); // Save the size of the file
        mSequence.reserve(size); //Preallocate string
        file.seekg (0, std::ios::beg); // Seek back to start of file
        
        getline(file, mHeader); //Get header
        if (mHeader[0] != '>')
        {
            throw FileLoadExcept();
        }
        mHeader = mHeader.substr(1);
        
        char *foundElement;
        while (file.get(currChar)) //Get sequence and check for validity
        {
            foundElement = std::find(std::begin(validChar), std::end(validChar), currChar);
            if (foundElement != std::end(validChar))
            {
                if (!isspace(currChar)) //Skip white space
                {
                    mSequence += currChar;
                }
            }
            else
            {
                throw FileLoadExcept();
            }
        }
        
        file.close();
    }
}

//Getters
const std::string& FASTAParser::GetHeader()
{
    return mHeader;
}

const std::string& FASTAParser::GetSequence()
{
    return mSequence;
}
