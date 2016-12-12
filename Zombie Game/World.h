#pragma once
#include "Singleton.h"
#include "Traits.h"
#include "Machine.h"
#include <vector>

class World : public Singleton<World>
{
    DECLARE_SINGLETON(World);
    
public:
    //Create new machine
    void AddNewZombie();
    void AddNewHuman();
    
    //Load in file-paths
    void LoadZombieFile(const std::string& filepath);
    void LoadHumanFile(const std::string& filepath);
    
    //Getters
    MachineState* GetZombieState(const int i) noexcept;
    Machine<ZombieTraits>* GetZombieMachine(const int i) noexcept;
    MachineState* GetHumanState(const int i) noexcept;
    Machine<HumanTraits>* GetHumanMachine(const int i) noexcept;
    const int GetZombieCount() noexcept;
    const int GetHumanCount() noexcept;
    const int GetMonth() noexcept;
    const std::string GetZombieFP() noexcept;
    const std::string GetHumanFP() noexcept;
    
    //Helper functions
    void ClearAll() noexcept;
    void IncrementMonth() noexcept;
    const bool Empty() noexcept;
    const int CheckOccupiedTile(int X, int Y) noexcept;
    const std::string ExtractFileName(const std::string& file) noexcept;
    void UnloadFP() noexcept;
    
    //Modify vectors
    void ConvertHumanToZombie() noexcept;
    void DestroyZombie() noexcept;
    void DestroyHuman() noexcept;
    
    //Store current zombie being targeted by CheckOccupiedTile
    int mIndexCurrentZombie, mIndexCurrentHuman;
protected:
    World() : Singleton(), mMonth(0), mZombieFP(""), mHumanFP(""){}

private:
    std::vector<Machine<HumanTraits>> mHumans;
    std::vector<Machine<ZombieTraits>> mZombies;
    std::vector<MachineState> mHumanStates;
    std::vector<MachineState> mZombieStates;
    
    //Filepaths
    std::string mZombieFP, mHumanFP;
    
    //Current month
    int mMonth;
};
