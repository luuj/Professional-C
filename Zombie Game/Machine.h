#pragma once
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include "Op.h"
#include "Exceptions.h"
#include <fstream>

// Defines state data accessible by the machine and ops
struct MachineState
{
	template <typename MachineTraits>
	friend class Machine;

	enum Facing { UP, RIGHT, DOWN, LEFT };
	MachineState()
		: mProgramCounter(1)
		, mActionsTaken(0)
		, mFacing(UP)
		, mTest(false)
        , mX(0)
        , mY(0)
	{ }

	~MachineState()
	{
	}

	// Active line number in behavior program
	int mProgramCounter;
	// Number of actions taken this turn
	int mActionsTaken;
	// Current facing of this character
	Facing mFacing;
	// Test flag for branches
	bool mTest;
    //X,Y Coordinates
    int mX, mY;

	int GetActionsPerTurn() const noexcept { return mActionsPerTurn; }
	bool GetInfect() const noexcept { return mInfectOnAttack; }
private:
	int mActionsPerTurn;
	bool mInfectOnAttack;
};

// Describes the machine which processes ops.
// Different policies dictate behavior possible for machine.
template <typename MachineTraits>
class Machine
{
public:
	// Load in all the ops for this machine from the specified file
	void LoadMachine(const std::string& filename);

	// Given the state, binds the trait parameters to it
	void BindState(MachineState& state);

	// Take a turn using this logic for the passed in state
	void TakeTurn(MachineState& state);
    
    void CreateOp(const std::string& commBuffer, const std::string& paramBuffer);

	// Destructor
	~Machine();
private:
	std::vector<std::shared_ptr<Op>> mOps;
};

template <typename MachineTraits>
void Machine<MachineTraits>::LoadMachine(const std::string& filename)
{
    std::string commBuffer, paramBuffer;
    mOps.clear();

    // Open the file for input
    std::ifstream file (filename, std::ios::in);
    if (file.is_open())
    {
        while (getline(file, commBuffer))
        {
            //Locate semicolon
            std::size_t pos = commBuffer.find(";");
            if (pos < commBuffer.length())
            {
                commBuffer = commBuffer.substr(0,pos);
                commBuffer.erase( std::remove_if( commBuffer.begin(), commBuffer.end(), ::isspace ), commBuffer.end() );
            }
            
            //Locate comma
            pos = commBuffer.find(",");
            if (pos < commBuffer.length())
            {
                //Extracting parameter
                for (std::size_t i=pos+1; i<commBuffer.length(); i++)
                {
                    if (isdigit(commBuffer[i]))
                    {
                        paramBuffer+= commBuffer[i];
                    }
                    else
                    {
                        //Invalid parameter
                        throw FileLoadExcept();
                    }
                }
                
                //Extracting OP
                commBuffer = commBuffer.substr(0, pos);
                commBuffer.erase( std::remove_if( commBuffer.begin(), commBuffer.end(), ::isspace ), commBuffer.end() );
            }
            
            //Create OPs
            CreateOp(commBuffer, paramBuffer);
        
            commBuffer.clear();
            paramBuffer.clear();
        }
        
        file.close();
    }
    else
    {
        throw FileLoadExcept();
    }
}

template <typename MachineTraits>
void Machine<MachineTraits>::BindState(MachineState& state)
{
	state.mActionsPerTurn = MachineTraits::ACTIONS_PER_TURN;
	state.mInfectOnAttack = MachineTraits::INFECT_ON_ATTACK;
}

template <typename MachineTraits>
void Machine<MachineTraits>::TakeTurn(MachineState& state)
{

	state.mActionsTaken = 0;
	while (state.mActionsTaken < MachineTraits::ACTIONS_PER_TURN)
	{
        try
        {
            if (mOps.size() == 0)
            {
                return;
            }
            
            mOps.at(state.mProgramCounter - 1)->Execute(state);
        }
        catch(InvalidRangedAttack ira)
        {
            throw InvalidRangedAttack();
        }
        catch(std::exception lm)
        {
            throw LineMissing();
        }
	}
}

template <typename MachineTraits>
Machine<MachineTraits>::~Machine()
{
	mOps.clear();
}

template <typename MachineTraits>
void Machine<MachineTraits>::CreateOp(const std::string &commBuffer, const std::string &paramBuffer)
{
    if (commBuffer.compare("attack") == 0)
    {
        mOps.push_back(std::make_shared<OpAttack>());
    }
    else if (commBuffer.compare("ranged_attack") == 0)
    {
        mOps.push_back(std::make_shared<OpRangedAttack>());
    }
    else if (commBuffer.compare("rotate") == 0)
    {
        //If param buffer is empty or is not 1 and not 2
        if ((paramBuffer.compare("") == 0) || ((paramBuffer.compare("1") != 0) && (paramBuffer.compare("0") != 0)))
        {
            throw InvalidOp();
        }
    	mOps.push_back(std::make_shared<OpRotate>(stoi(paramBuffer)));
    }
    else if (commBuffer.compare("forward") == 0)
    {
        mOps.push_back(std::make_shared<OpForward>());
    }
    else if (commBuffer.compare("endturn") == 0)
    {
        mOps.push_back(std::make_shared<OpEndTurn>());
    }
    else if (commBuffer.compare("test_human") == 0)
    {
        if ((paramBuffer.compare("") == 0) || ((paramBuffer.compare("1") != 0) && (paramBuffer.compare("2") != 0)))
        {
            throw InvalidOp();
        }
        mOps.push_back(std::make_shared<OpTestHuman>(stoi(paramBuffer)));
    }
    else if (commBuffer.compare("test_wall") == 0)
    {
        mOps.push_back(std::make_shared<OpTestWall>());
    }
    else if (commBuffer.compare("test_zombie") == 0)
    {
        if ((paramBuffer.compare("") == 0) || ((paramBuffer.compare("1") != 0) && (paramBuffer.compare("2") != 0)))
        {
            throw InvalidOp();
        }
        mOps.push_back(std::make_shared<OpTestZombie>(stoi(paramBuffer)));
    }
    else if (commBuffer.compare("test_random") == 0)
    {
        mOps.push_back(std::make_shared<OpTestRandom>());
    }
    else if (commBuffer.compare("test_passable") == 0)
    {
        mOps.push_back(std::make_shared<OpTestPassable>());
    }
    else if (commBuffer.compare("je") == 0)
    {
        if (paramBuffer.compare("") == 0)
        {
            throw InvalidOp();
        }
        mOps.push_back(std::make_shared<OpJE>(stoi(paramBuffer)));
    }
    else if (commBuffer.compare("jne") == 0)
    {
        if (paramBuffer.compare("") == 0)
        {
            throw InvalidOp();
        }
        mOps.push_back(std::make_shared<OpJNE>(stoi(paramBuffer)));
    }
    else if (commBuffer.compare("goto") == 0)
    {
        if (paramBuffer.compare("") == 0)
        {
            throw InvalidOp();
        }
        mOps.push_back(std::make_shared<OpGoto>(stoi(paramBuffer)));
    }
    else
    {
        throw InvalidOp();
    }
}
