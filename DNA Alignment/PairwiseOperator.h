#pragma once
#include "FASTAParser.h"
#include <vector>

class PairwiseOperator
{
public:
    PairwiseOperator(const FASTAParser& inpA, const FASTAParser& inpB, const std::string& outFile);
    void RunNeedleman();

    short FindMax(short leftVal, short aboveVal, short aboveLeftVal, char& direction);
    const void CreateOutputFile(std::string &sequenceAResult, std::string &sequenceBResult, short score);

private:
    FASTAParser mInputA, mInputB;
    std::vector<std::vector<short>> mScoreCell;
    std::vector<std::vector<char>> mDirectionCell;
    std::string outputFileName;
    
    //Scores
    const short mMatch = 1;
    const short mMismatch = -1;
    const short mGap = -1;
};

enum Direction : char
{
    AboveLeft, Above, Left
};
