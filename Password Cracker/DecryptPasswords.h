#pragma once
#include <string>
#include <unordered_map>
#include <map>
#include <sys/stat.h>
#include <fstream>
#include <vector>
#include <iostream>
#include "CalculateHash.h"
#include "Timer.h"
#include "tbb/parallel_invoke.h"

/*************************************************
* PA2 Part 2 & 3: Dictionary Lookup & Brute-force
*************************************************/

class DecryptPasswords
{
public:
    DecryptPasswords(char* pF, std::unordered_map<std::string, std::string> inMap);
    ~DecryptPasswords();
    
    bool checkFileExists();
    void decrypt();
    std::string convertBase36(int val);
    void bruteForceDecrypt();
    void writeFile();
    
    struct Password
    {
        Password(){}
        Password(std::string h, std::string o, int i) : hash(h), original(o), index(i){}
        ~Password(){}
        std::string hash;
        std::string original;
        int index;
    };
    
    void bruteForceHelper(int start, int end);

private:
    std::unordered_map<std::string, std::string> dictionaryMap;
    std::map<int, Password> solvedPasswords;
    std::vector<Password> unsolvedPasswords;
    char* passwordFile;
};