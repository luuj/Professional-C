#include "DNATranslator.h"
#include <iostream>
#include <ctype.h>

DNATranslator::DNATranslator(const std::string& sequence)
{
    //Initialize amino acid counts & state machine
    for (int i=0; i<20; i++)
    {
        mAminoCount.push_back(0);
    }
    
    mStateMachine = {
    //St| T| C| A| G|
    //0  Start Codon: A
        { 0, 0, 1, 0 },
    //1  Start Codon: T
        { 2, 0, 1, 0 },
    //2  Start Codon: G, Produce M
        { 0, 0, 1, M},
    //3  First letter of next amino acid
        { 4, 9, 14, 19},
    //4  T**
        { 5, 6, 7, 8},
    //5  TT*
        { F, F, L, L},
    //6  TC*
        { S, S, S, S},
    //7  TA*
        { Y, Y, 0, 0},
    //8  TG*
        { C, C, 0, W},
    //9  C**
        { 10, 11, 12, 13},
    //10 CT*
        { L, L, L, L},
    //11 CC*
        { P, P, P, P},
    //12 CA*
        { H, H, Q, Q},
    //13 CG*
        { R, R, R, R},
    //14 A**
        { 15, 16, 17, 18},
    //15 AT*
        { I, I, I, M},
    //16 AC*
        { T, T, T, T},
    //17 AA*
        { N, N, K, K},
    //18 AG*
        { S, S, R, R},
    //19 G*
        { 20, 21, 22, 23},
    //20 GT*
        { V, V, V, V},
    //21 GC*
        { A, A, A, A},
    //22 GA*
        { D, D, E, E},
    //23 GG*
        { G, G, G, G}
    };
    
    //Run the state machine
    int currState = 0;
    std::vector<char> currTransition;

    for (int i=0; i<sequence.length()-1; i++)
    {
        if (sequence.at(i) == '\n')
        {
            continue;
        }

        currTransition = mStateMachine.at(currState);
        currState = FindNextState(sequence.at(i), currTransition);

        //If a letter is found, go to state 3
        if (currState > 23)
        {
            ++mAminoCount[currState-24];
            currState = 3;
        }
    }
}

const char DNATranslator::FindNextState(char currNucleotide, std::vector<char> &currTransition)
{
    switch (currNucleotide)
    {
        case 'T':
        {
            return currTransition[0];
            break;
        }
        case 'C':
        {
            return currTransition[1];
            break;
        }
        case 'A':
        {
            return currTransition[2];
            break;
        }
        case 'G':
        {
            return currTransition[3];
            break;
        }
        default:
        {
            std::cout << "Error finding nucleotide" << std::endl;
        }
    }
    
    return -1;
}

const std::vector<int>& DNATranslator::GetAminoCount()
{
    return mAminoCount;
}



