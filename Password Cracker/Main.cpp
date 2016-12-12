// Main.cpp : Defines the entry point for the console application.
//
#include <iostream>
#include "CalculateHash.h"
#include "Dictionary.h"
#include "DecryptPasswords.h"

int main(int argc, char* argv[])
{
    //Check for arguments
    if (argc == 1)
    {
        std::cout << "Error. No command-line arguments found." << std::endl;
    }
    
    //Output hash of input
    if (argc == 2)
    {
        std::cout << CalculateHash::calculate(argv[1]) << std::endl;
    }
    
    //Load dictionary and decrypt passwords
    if (argc == 3)
    {
        Dictionary dictionary(argv[1]);
        if (!dictionary.checkFileExists())
        {
            std::cout << "Error. Dictionary file not found." << std::endl;
            return -1;
        }
        dictionary.loadDictionary();
        
        DecryptPasswords decrypter(argv[2], dictionary.passMap());
        if (!decrypter.checkFileExists())
        {
            std::cout << "Error. Password file not found." << std::endl;
            return -1;
        }
        decrypter.decrypt();
        decrypter.bruteForceDecrypt();
        decrypter.writeFile();
    }
    
	return 0;
}

