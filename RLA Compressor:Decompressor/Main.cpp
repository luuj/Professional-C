// Main.cpp : Defines the entry point for the console application.
//

#include "RleTests.h"
#include "RleData.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sstream>
#include "RleFile.h"

void Part1Tests()
{
	TestFixtureFactory::theInstance().runTests();
}

int main(int argc, char* argv[])
{
	if (argc < 2)
	{
		// Just the tests
		Part1Tests();
	}
	else
	{
		// TODO: Get the file name from argv[1] and either compress/decompress
        std::string fileName = argv[1];
        RleFile rf = *new RleFile();
        
        //Check file extension to determine whether to extract or create archive
        if (fileName.substr(fileName.find_last_of(".") + 1) == "rl1")
        {
            rf.ExtractArchive(argv[1]);
        }
        else
        {
            rf.CreateArchive(argv[1]);
        }
        
	}
	return 0;
}

