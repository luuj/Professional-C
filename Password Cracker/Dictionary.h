#pragma once

#include "Sha1.h"
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include <string>
#include "Timer.h"
#include "CalculateHash.h"

/******************************
* PA2 Part 2: Dictionary Lookup
******************************/

class Dictionary
{
public:
    Dictionary(char* dictionaryFile);
    ~Dictionary();
    
    bool checkFileExists();
    void loadDictionary();
    std::unordered_map<std::string, std::string> passMap();
private:
    char* dictionaryFile;
    std::unordered_map<std::string, std::string> lookUpMap;
};