#pragma once
#include <string>

class FASTAParser
{
public:
    FASTAParser();
    FASTAParser(const std::string& filePath);
    const std::string& GetHeader();
    const std::string& GetSequence();

private:
    std::string mHeader, mSequence;
};
