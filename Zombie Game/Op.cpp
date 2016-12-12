#include "Op.h"
#include "Machine.h"
#include "World.h"
#include <iostream>

void Op::DebugOutput(MachineState& state)
{
	std::cout << state.mProgramCounter << ":" << mOpName << "," << mParam << ", Facing " << state.mFacing << ", Test " << state.mTest
    << " X,Y " << state.mX << state.mY << std::endl;
}

void OpRotate::Execute(MachineState& state)
{
	switch (state.mFacing)
	{
	case (MachineState::UP) :
		if (mParam == 0)
		{
			state.mFacing = MachineState::RIGHT;
		}
		else
		{
			state.mFacing = MachineState::LEFT;
		}
		break;
	case (MachineState::RIGHT) :
		if (mParam == 0)
		{
			state.mFacing = MachineState::DOWN;
		}
		else
		{
			state.mFacing = MachineState::UP;
		}
		break;
	case (MachineState::DOWN) :
		if (mParam == 0)
		{
			state.mFacing = MachineState::LEFT;
		}
		else
		{
			state.mFacing = MachineState::RIGHT;
		}
		break;
	default:
	case (MachineState::LEFT) :
		if (mParam == 0)
		{
			state.mFacing = MachineState::UP;
		}
		else
		{
			state.mFacing = MachineState::DOWN;
		}
		break;
	}

	state.mProgramCounter++;
	state.mActionsTaken++;
}

void OpGoto::Execute(MachineState& state)
{
	state.mProgramCounter = mParam;
}

void OpTestWall::Execute(MachineState& state)
{
    state.mTest = InvalidTile(state);
	state.mProgramCounter++;
}

void OpJE::Execute(MachineState& state)
{
    if (state.mTest)
    {
        state.mProgramCounter = mParam;
        return;
    }
    
	state.mProgramCounter++;
}

void OpTestRandom::Execute(MachineState& state)
{
    srand(static_cast<unsigned>(std::time(0)));
    state.mTest = rand() % 2;
	state.mProgramCounter++;
}

void OpForward::Execute(MachineState& state)
{
    //Check for wall
    if (!InvalidTile(state))
    {
        //Check for occupied space
        switch(state.mFacing)
        {
        case (MachineState::UP):
            if (!World::get().CheckOccupiedTile(state.mX, state.mY-1))
            {
                --state.mY;
            }
            break;
        case (MachineState::RIGHT):
            if (!World::get().CheckOccupiedTile(state.mX+1, state.mY))
            {
                ++state.mX;
            }
            break;
        case (MachineState::DOWN):
            if (!World::get().CheckOccupiedTile(state.mX, state.mY+1))
            {
                ++state.mY;
            }
            break;
        case (MachineState::LEFT):
            if (!World::get().CheckOccupiedTile(state.mX-1, state.mY))
            {
                --state.mX;
            }
            break;
        }
    }
    
    state.mActionsTaken++;
	state.mProgramCounter++;
}

//Helper function - check to see if facing a wall
const bool Op::InvalidTile(const MachineState& state) noexcept
{
    switch(state.mFacing)
    {
    case (MachineState::UP):
        return (state.mY == 0) ? true : false;
        break;
    case (MachineState::RIGHT):
        return (state.mX == 19) ? true : false;
        break;
	case (MachineState::DOWN):
        return (state.mY == 19) ? true : false;
        break;
	case (MachineState::LEFT):
        return (state.mX == 0) ? true : false;
        break;
    }
}

//Helper function - check for zombie or human in front
const int Op::TestMachine(const MachineState &state) noexcept
{
    switch(state.mFacing)
    {
    case (MachineState::UP):
        return World::get().CheckOccupiedTile(state.mX, state.mY-mParam);
        break;
    case (MachineState::RIGHT):
        return World::get().CheckOccupiedTile(state.mX+mParam, state.mY);
        break;
    case (MachineState::DOWN):
        return World::get().CheckOccupiedTile(state.mX, state.mY+mParam);
        break;
    case (MachineState::LEFT):
        return World::get().CheckOccupiedTile(state.mX-mParam, state.mY);
        break;
    }
}

void OpEndTurn::Execute(MachineState &state)
{
    state.mActionsTaken = 10;
    state.mProgramCounter++;
}

void OpJNE::Execute(MachineState& state)
{
    if (!state.mTest)
    {
        state.mProgramCounter = mParam;
        return;
    }
    
	state.mProgramCounter++;
}

void OpTestHuman::Execute(MachineState& state)
{
    state.mTest = (TestMachine(state) == 2) ? true : false;
    state.mProgramCounter++;
}

void OpTestZombie::Execute(MachineState &state)
{
    state.mTest = (TestMachine(state) == 1) ? true : false;
    state.mProgramCounter++;	
}

void OpTestPassable::Execute(MachineState &state)
{
    state.mProgramCounter++;
    
    if (InvalidTile(state))
    {
        state.mTest = false;
        return;
    }
    
    int testResult = TestMachine(state);
    if (testResult == 0)
    {
        state.mTest = true;
    }
    else
    {
        state.mTest = false;
    }
}

void OpAttack::Execute(MachineState &state)
{
    bool isZombie = state.GetInfect();
    int testResult;
    state.mProgramCounter++;
    state.mActionsTaken++;
    
    //If you attack a wall, do nothing
    if (InvalidTile(state))
    {
        return;
    }
    
    //If you are a zombie
    if (isZombie)
    {
        testResult = TestMachine(state);
        //Nothing in front of you
        if (testResult == 0)
        {
            return;
        }
        //If you attack another zombie, nothing happens
        else if (testResult == 1)
        {
            return;
        }
        //If you attack a human, they turn into a zombie
        else if (testResult == 2)
        {
            World::get().ConvertHumanToZombie();
        }
    }
    //If you are a human
    else
    {
        testResult = TestMachine(state);
        //Nothing in front of you
        if (testResult == 0)
        {
            return;
        }
        //If you attack a zombie, it dies
        else if (testResult == 1)
        {
            World::get().DestroyZombie();
        }
        else if (testResult == 2)
        {
            World::get().DestroyHuman();
        }
    }
}

void OpRangedAttack::Execute(MachineState &state)
{
    //Check for human-use only
    bool isZombie = state.GetInfect();
    if (isZombie)
    {
        throw InvalidRangedAttack();
    }
    
    state.mProgramCounter++;
    state.mActionsTaken++;
    
    int testResult = TestMachine(state);
    if (testResult == 0)
    {
        return;
    }
    else if (testResult == 1)
    {
        World::get().DestroyZombie();
    }
    else if (testResult == 2)
    {
        World::get().DestroyHuman();
    }

}

