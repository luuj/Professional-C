#pragma once

struct MachineState;

// Abstract Base Class
class Op
{
public:
	Op(const char* OpName, int parameter)
		: mOpName(OpName)
		, mParam(parameter)
	{ }

	void DebugOutput(MachineState& state);

	virtual void Execute(MachineState& state) = 0;
        
    const bool InvalidTile(const MachineState& state) noexcept;
    
    const int TestMachine(const MachineState& state) noexcept;
	
	virtual ~Op() { }
protected:
	int mParam;
	const char* mOpName;
};

// Defines the rotate operation
// Param of 0 means rotate clockwise
// Param of 1 means rotate counter-clockwise
struct OpRotate : Op
{
	OpRotate(int parameter)
		: Op("OpRotate", parameter)
	{ }

	virtual void Execute(MachineState& state) override;
};

// Defines the goto operation
// Parameter determines the line number
struct OpGoto : Op
{
	OpGoto(int parameter)
		: Op("OpGoto", parameter)
	{ }

	virtual void Execute(MachineState& state) override;
};

//Defines the jump condition operation
//Parameter determines the line number
struct OpJE : Op
{
    OpJE(int parameter)
        : Op("OpJE", parameter)
    { }
    
	virtual void Execute(MachineState& state) override;
};

//Defines the test-wall operation
//Sets mTest to true if facing wall
struct OpTestWall : Op
{
    OpTestWall()
        : Op("OpTestWall", -1)
    { }
    
	virtual void Execute(MachineState& state) override;
};

//Defines the test-random operation
//Sets mTest to true or false randomly
struct OpTestRandom : Op
{
    OpTestRandom()
        : Op("OpTestRandom", -1)
    { }
    
    virtual void Execute(MachineState& state) override;
};

//Defines the forward operation
//Moves machine forward if no obstacles in the way
struct OpForward : Op
{
    OpForward()
        : Op("OpForward", -1)
    { }
    
    virtual void Execute(MachineState& state) override;
};

//Defines the end-turn operation
//Ends the current machine's turn
struct OpEndTurn : Op
{
    OpEndTurn()
        : Op("OpEndTurn", -1)
    { }
    
    virtual void Execute(MachineState& state) override;
};

//Defines the JNE operation
//Jumps to the line given if flag is false
struct OpJNE : Op
{
    OpJNE(int parameter)
        : Op("OpJNE", parameter)
    { }
    
	virtual void Execute(MachineState& state) override;
};

//Defines the test-human operation
//Tests to see if a human is n tiles in front, n= 1 or 2
struct OpTestHuman : Op
{
    OpTestHuman(int parameter)
        : Op("OpTestHuman", parameter)
    { }
    
	virtual void Execute(MachineState& state) override;
};

//Defines the test-zombie operation
//Tests to see if a zombie is n tiles in front, n= 1 or 2
struct OpTestZombie : Op
{
    OpTestZombie(int parameter)
        : Op("OpTestZombie", parameter)
    { }
    
	virtual void Execute(MachineState& state) override;
};

//Defines the test-passable operation
//Sets test flag to true if facing an open tile
struct OpTestPassable : Op
{
    OpTestPassable()
        : Op("OpTestPassable", 1)
    { }
    
    virtual void Execute(MachineState& state) override;
};

//Defines the attack operation
//If a zombie uses it and kills a human, they become a zombie
//If a human uses it and kills a zombie, the zombie disappears
struct OpAttack : Op
{
    OpAttack()
        : Op("OpAttack", 1)
    { }
    
    virtual void Execute(MachineState& state) override;
};

//Defines the ranged attack operation
//Only a human can use it, attacks two blocks away
struct OpRangedAttack : Op
{
    OpRangedAttack()
        : Op("OpRangedAttack", 2)
    { }
    
    virtual void Execute(MachineState& state) override;
};
