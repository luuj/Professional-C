#include "Dictionary.h"

Dictionary::Dictionary(char* dF)
{
    Dictionary::dictionaryFile = dF;
}

Dictionary::~Dictionary(){}

void Dictionary::loadDictionary()
{
    std::ifstream infile(dictionaryFile);
    lookUpMap.rehash(100000);
    
    std::string input;
    char* convertedInput;
    
    //Start timer
    Timer timer;
    timer.start();

    //Read passwords from d8.txt
    //Hash the passwords and insert into map
    while (std::getline(infile, input))
    {
        convertedInput = const_cast<char*>(input.c_str());
        std::pair<std::string, std::string> dictionaryTerm (std::string(CalculateHash::calculate(convertedInput)),input);
        lookUpMap.insert(dictionaryTerm);
    }
    
    //Stop timer
    double elapsed = timer.getElapsed();
    std::cout << "Dictionary load time: " << elapsed << " sec" << std::endl;
    infile.close();
}

//File-checker code from StackOverflow for optimal runtime
bool Dictionary::checkFileExists()
{
    struct stat buffer;
    return (stat (dictionaryFile, &buffer) == 0);
}

std::unordered_map<std::string, std::string> Dictionary::passMap()
{
    return lookUpMap;
}


