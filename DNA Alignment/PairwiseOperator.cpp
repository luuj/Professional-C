#include "PairwiseOperator.h"
#include <string>
#include <iostream>
#include <fstream>
#include "Timer.h"

PairwiseOperator::PairwiseOperator(const FASTAParser& inpA, const FASTAParser& inpB, const std::string& outFile) : mInputA(inpA), mInputB(inpB), outputFileName(outFile){}

void PairwiseOperator::RunNeedleman()
{
    std::string sequenceA = mInputA.GetSequence();
    std::string sequenceB = mInputB.GetSequence();
    int lengthB = static_cast<int>(sequenceB.length()) + 1;
    int lengthA = static_cast<int>(sequenceA.length()) + 1;
    
    //Timer start
    Timer time = *new Timer();
    time.Start();
    
    //Resize grid
    mScoreCell.resize(lengthB);
    mDirectionCell.resize(lengthB);
    
    for (int i=0; i<lengthB; i++)
    {
        mScoreCell[i].resize(lengthA);
        mDirectionCell[i].resize(lengthA);
    }
    
    //Initialize grid
    mScoreCell[0][0] = 0;
    for (int i=1; i<lengthA; i++)
    {
        mScoreCell[0][i] = -i;
        mDirectionCell[0][i] = Left;
    }
    
    for (int i=1; i<lengthB; i++)
    {
        mScoreCell[i][0] = -i;
        mDirectionCell[i][0] = Above;
    }

    //Calculate score values
    short leftVal, aboveVal, aboveLeftVal;
    
    for (int row=1; row<lengthB; row++)
    {
        for (int col=1; col<lengthA; col++)
        {
            leftVal = mScoreCell[row][col-1] + mGap;
            aboveVal = mScoreCell[row-1][col] + mGap;
            aboveLeftVal = (sequenceA.at(col-1) == sequenceB.at(row-1)) ? mScoreCell[row-1][col-1] + mMatch : mScoreCell[row-1][col-1] + mMismatch;
            mScoreCell[row][col] = FindMax(leftVal, aboveVal, aboveLeftVal, mDirectionCell[row][col]);
        }
    }
    
    int currRow = lengthB-1;
    int currCol = lengthA-1;
    std::string sequenceAResult, sequenceBResult;
    sequenceBResult.reserve(lengthB);
    sequenceAResult.reserve(lengthA);

    //Traceback
    while (currRow !=0 || currCol !=0)
    {
        if (mDirectionCell[currRow][currCol] == AboveLeft)
        {
            sequenceAResult += sequenceA[currCol-1];
            sequenceBResult += sequenceB[currRow-1];
            --currCol;
            --currRow;
        }
        else if (mDirectionCell[currRow][currCol] == Left)
        {
            sequenceAResult += sequenceA[currCol-1];
            sequenceBResult += '_';
            --currCol;
        }
        else
        {
            sequenceAResult += '_';
            sequenceBResult += sequenceB[currRow-1];
            --currRow;
        }
    }
    
    std::reverse(sequenceAResult.begin(), sequenceAResult.end());
    std::reverse(sequenceBResult.begin(), sequenceBResult.end());
    short finalScore = mScoreCell[lengthB-1][lengthA-1];
    CreateOutputFile(sequenceAResult, sequenceBResult, finalScore);
    
    //Stop timer
    double timeTaken = time.GetElapsed();
    std::cout << "Time taken: " << timeTaken << " sec" << std::endl;
}

short PairwiseOperator::FindMax(short leftVal, short aboveVal, short aboveLeftVal, char& direction)
{
    short tempMax;
    if (aboveLeftVal >= leftVal)
    {
        tempMax = aboveLeftVal;
        direction = AboveLeft;
    }
    else
    {
        tempMax = leftVal;
        direction = Left;
    }
    
    if (tempMax < aboveVal)
    {
        tempMax = aboveVal;
        direction = Above;
    }
    
    return tempMax;
}

const void PairwiseOperator::CreateOutputFile(std::string &sequenceAResult, std::string &sequenceBResult, short score)
{
    std::ofstream outfile(outputFileName);
    outfile << "A: " << mInputA.GetHeader() << "\n";
    outfile << "B: " << mInputB.GetHeader() << "\n";
    outfile << "Score: " << score << "\n\n";

    int currIndex = 0;
    int oldIndex = 0;
    int stringSize = static_cast<int>(sequenceBResult.length());
    
    while (currIndex != stringSize)
    {
        oldIndex = currIndex;
        //Write Sequence A
        for (int i=0; i<70; i++)
        {
            outfile << sequenceAResult[currIndex];
            ++currIndex;
            
            if (currIndex >= stringSize)
            {
                break;
            }
        }
        outfile << "\n";
        currIndex = oldIndex;
        
        //Write match lines
        for (int i=0; i<70; i++)
        {
            if (sequenceAResult[currIndex] == sequenceBResult[currIndex])
            {
                outfile << '|';
            }
            else
            {
                outfile << " ";
            }
            
            ++currIndex;
            
            if (currIndex >= stringSize)
            {
                break;
            }
        }
        outfile << "\n";
        currIndex = oldIndex;
        
        //Write Sequence B
        for (int i=0; i<70; i++)
        {
            outfile << sequenceBResult[currIndex];
            ++currIndex;
            
            if (currIndex >= stringSize)
            {
                break;
            }
        }
        outfile << "\n\n";
    }
    
    outfile.close();
}

