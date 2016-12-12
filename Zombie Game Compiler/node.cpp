#include "node.h"
#include <sstream>
#include <iostream>

//NBlock
void NBlock::AddStatement(NStatement* statement)
{
	mStatements.push_back(statement);
}

void NBlock::CodeGen(CodeContext& context) const
{
	for (auto i : mStatements)
    {
        i->CodeGen(context);
    }
    
    //Check if main loop
    if (mbMainBlock)
    {
        context.mOps.push_back("goto,1");
        context.mGoTos.insert( std::pair<int,int>(static_cast<int>(context.mOps.size()),1) );
    }
}

//NNumeric
NNumeric::NNumeric(std::string& value)
{
	mvalue = std::atoi(value.c_str());
}

//NRotate
NRotate::NRotate(NNumeric* dir)
	: mDir(dir)
{
}

void NRotate::CodeGen(CodeContext& context) const
{
	if (mDir->mvalue == 0)
	{
		context.mOps.push_back("rotate,0");
	}
	else if (mDir->mvalue == 1)
	{
		context.mOps.push_back("rotate,1");
	}
}

//NAttack
NAttack::NAttack()
{
}

void NAttack::CodeGen(CodeContext &context) const
{
    context.mOps.push_back("attack");
}

//NRangedAttack
NRangedAttack::NRangedAttack()
{
}

void NRangedAttack::CodeGen(CodeContext &context) const
{
    context.mOps.push_back("ranged_attack");
}

//NForward
NForward::NForward()
{
}

void NForward::CodeGen(CodeContext &context) const
{
    context.mOps.push_back("forward");
}

//NIf/NElse
NIfElse::NIfElse(NBoolean* boolean, NBlock* ifblock, NBlock* elseblock)
    : mBool(boolean), mIfBlock(ifblock), mElseBlock(elseblock)
{
}

void NIfElse::CodeGen(CodeContext &context) const
{
    int ifLocation, elseLocation;
    std::string ifFinalString, elseFinalString;
    
    //Test flag and temporary je
    mBool->CodeGen(context);
    context.mOps.push_back("je,");
    ifLocation = static_cast<int>(context.mOps.size())-1;
    
    //Else block code and temporary goto
    mElseBlock->CodeGen(context);
    context.mOps.push_back("goto,");
    elseLocation = static_cast<int>(context.mOps.size())-1;

    //Fix je location
    ifFinalString = "je," + std::to_string(context.mOps.size()+1);
    context.mOps.at(ifLocation) = ifFinalString;
    
    //If block code
    mIfBlock->CodeGen(context);
    
    elseFinalString = "goto," + std::to_string(context.mOps.size()+1);
    context.mOps.at(elseLocation) = elseFinalString;
    
    //Track Go-tos
    context.mGoTos.insert( std::pair<int,int>(elseLocation+1,context.mOps.size()+1) );
}

//NIsHuman
NIsHuman::NIsHuman(NNumeric* val)
    : mVal(val)
{
}

void NIsHuman::CodeGen(CodeContext &context) const
{
    if (mVal->mvalue == 1)
    {
        context.mOps.push_back("test_human,1");
    }
    else
    {
        context.mOps.push_back("test_human,2");
    }
}

//NIsPassable
NIsPassable::NIsPassable()
{
}

void NIsPassable::CodeGen(CodeContext &context) const
{
    context.mOps.push_back("test_passable");
}

//NIsRandom
NIsRandom::NIsRandom()
{
}

void NIsRandom::CodeGen(CodeContext &context) const
{
    context.mOps.push_back("test_random");
}

//NIsZombie
NIsZombie::NIsZombie(NNumeric* val)
    : mVal(val)
{
}

void NIsZombie::CodeGen(CodeContext &context) const
{
    if (mVal->mvalue == 1)
    {
        context.mOps.push_back("test_zombie,1");
    }
    else
    {
        context.mOps.push_back("test_zombie,2");
    }
}


