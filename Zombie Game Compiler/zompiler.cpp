// zompiler.cpp : Defines the entry point for the console application.
//

#ifdef _WIN32
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <iostream>
#include "node.h"
#include <fstream>

extern int g_LineNumber;
extern NBlock* g_MainBlock;

extern int zompilerparse();
extern FILE* zompilerin;

int main(int argc, char* argv[])
{
	// Read input from the first command line parameter
	zompilerin = fopen(argv[1], "r");
	
	// Start the parse
	zompilerparse();
	
    CodeContext myContext = *new CodeContext();
    
    if (g_MainBlock != nullptr)
    {
        g_MainBlock->CodeGen(myContext);
    }
	
	// Close the file stream
	fclose(zompilerin);
    
    //Optimize go-tos
    int first, second;
    std::map<int,int>::iterator it;
    for (auto i : myContext.mGoTos)
    {
        first = i.first;
        second = i.second;
        
        do
        {
            it = myContext.mGoTos.find(second);
            if (it != myContext.mGoTos.end())
            {
                second = it->second;
            }
        }
        while (it != myContext.mGoTos.end());
        
        myContext.mOps.at(first-1) = "goto," + std::to_string(second);
    }
    
    //Create out.zom
    std::ofstream outfile("out.zom");
    for (int i=0; i<myContext.mOps.size(); i++)
    {
        outfile << myContext.mOps.at(i) << "\n";
    }
    
    outfile.close();

	return 0;
}

void zompilererror(const char* s)
{
	std::cout << s << " on line " << g_LineNumber << std::endl;
}

