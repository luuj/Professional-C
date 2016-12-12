#include "World.h"

void World::AddNewZombie()
{
	MachineState zombieTestState;
	Machine<ZombieTraits> zombieMachine;
    
    zombieMachine.LoadMachine(mZombieFP);
    zombieMachine.BindState(zombieTestState);
    mZombies.push_back(zombieMachine);
    mZombieStates.push_back(zombieTestState);
}

void World::AddNewHuman()
{
	MachineState humanTestState;
	Machine<HumanTraits> humanMachine;

    humanMachine.LoadMachine(mHumanFP);
    humanMachine.BindState(humanTestState);
    mHumans.push_back(humanMachine);
    mHumanStates.push_back(humanTestState);
}

void World::LoadHumanFile(const std::string &filepath)
{
	Machine<HumanTraits> humanMachine;
    humanMachine.LoadMachine(filepath);

    mHumanFP = filepath;
}

void World::LoadZombieFile(const std::string &filepath)
{
	Machine<ZombieTraits> zombieMachine;
    zombieMachine.LoadMachine(filepath);

    mZombieFP = filepath;
}

MachineState* World::GetZombieState(const int i) noexcept
{
    return &mZombieStates.at(i);
}

Machine<ZombieTraits>* World::GetZombieMachine(const int i) noexcept
{
    return &mZombies.at(i);
}

MachineState* World::GetHumanState(const int i) noexcept
{
    return &mHumanStates.at(i);
}

Machine<HumanTraits>* World::GetHumanMachine(const int i) noexcept
{
    return &mHumans.at(i);
}

const int World::GetZombieCount() noexcept
{
    return static_cast<int>(mZombies.size());
}

const int World::GetHumanCount() noexcept
{
    return static_cast<int>(mHumans.size());
}

void World::ClearAll() noexcept
{
    mZombies.clear();
    mZombieStates.clear();
    mHumans.clear();
    mHumanStates.clear();
    mMonth = 0;
}

const bool World::Empty() noexcept
{
    return (mZombies.size() == 0 && mHumans.size() == 0) ? true : false;
}

const int World::CheckOccupiedTile(int X, int Y) noexcept
{
    mIndexCurrentHuman = -1;
    mIndexCurrentZombie = -1;
    
    for (auto i : mZombieStates)
    {
        mIndexCurrentZombie++;
        if (i.mX == X && i.mY == Y)
        {
            return 1;
        }
    }
    
    for (auto i : mHumanStates)
    {
        mIndexCurrentHuman++;
        if (i.mX == X && i.mY == Y)
        {
            return 2;
        }
    }
    
    mIndexCurrentHuman = -1;
    mIndexCurrentZombie = -1;
    return 0;
}

void World::ConvertHumanToZombie() noexcept
{
    //Save human position and mFacing and add zombie
    MachineState humanPlaceholder = mHumanStates.at(mIndexCurrentHuman);
    AddNewZombie();
    mZombieStates.back().mX = humanPlaceholder.mX;
    mZombieStates.back().mY = humanPlaceholder.mY;
    mZombieStates.back().mFacing = humanPlaceholder.mFacing;

    DestroyHuman();
}

void World:: DestroyZombie() noexcept
{
    mZombies.erase(mZombies.begin() + mIndexCurrentZombie);
    mZombieStates.erase(mZombieStates.begin() + mIndexCurrentZombie);
}

void World::DestroyHuman() noexcept
{
    mHumans.erase(mHumans.begin() + mIndexCurrentHuman);
    mHumanStates.erase(mHumanStates.begin() + mIndexCurrentHuman);
}

void World::IncrementMonth() noexcept
{
    mMonth++;
}

const int World::GetMonth() noexcept
{
    return mMonth;
}

const std::string World::GetZombieFP() noexcept
{
    return ExtractFileName(mZombieFP);
}

const std::string World::GetHumanFP() noexcept
{
    return ExtractFileName(mHumanFP);
}

const std::string World::ExtractFileName(const std::string& file) noexcept
{
    std::size_t index = file.find_last_of("/");
    if (index == std::string::npos)
    {
        return file;
    }
    
    return file.substr(index+1);
}

void World::UnloadFP() noexcept
{
    mZombieFP = "";
    mHumanFP = "";
}

