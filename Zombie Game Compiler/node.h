#pragma once

#include <list>
#include <vector>
#include <string>
#include <map>

struct CodeContext
{
	std::vector<std::string> mOps;
    std::map<int,int> mGoTos;
};

class Node
{
public:
	virtual void CodeGen(CodeContext& context) const = 0;
};

//Base types
class NNumeric : public Node
{
public:
	NNumeric(std::string& value);
	int mvalue;
	virtual void CodeGen(CodeContext& context) const override { }
};

class NStatement : public Node
{
};

class NBoolean : public Node
{
};

class NBlock : public Node
{
public:
	NBlock() { mbMainBlock = false; }
	virtual void CodeGen(CodeContext& context) const override;
	void AddStatement(NStatement* statement);
	void SetMainBlock() { mbMainBlock = true; }
private:
	std::list<NStatement*> mStatements;
	bool mbMainBlock;
};

//Statement Types
class NRotate : public NStatement
{
public:
	NRotate(NNumeric* dir);
	virtual void CodeGen(CodeContext& context) const override;
private:
	NNumeric* mDir;
};

class NAttack : public NStatement
{
public:
	NAttack();
	virtual void CodeGen(CodeContext& context) const override;
};

class NRangedAttack : public NStatement
{
public:
	NRangedAttack();
	virtual void CodeGen(CodeContext& context) const override;
};

class NForward : public NStatement
{
public:
	NForward();
	virtual void CodeGen(CodeContext& context) const override;
};

class NIfElse : public NStatement
{
public:
    NIfElse(NBoolean* boolean, NBlock* ifblock, NBlock* elseblock);
    virtual void CodeGen(CodeContext& context) const override;
private:
    NBoolean* mBool;
    NBlock* mIfBlock;
    NBlock* mElseBlock;
};

//Boolean types
class NIsHuman : public NBoolean
{
public:
    NIsHuman(NNumeric* val);
	virtual void CodeGen(CodeContext& context) const override;
private:
    NNumeric* mVal;
};

class NIsPassable : public NBoolean
{
public:
    NIsPassable();
	virtual void CodeGen(CodeContext& context) const override;
};

class NIsRandom : public NBoolean
{
public:
    NIsRandom();
	virtual void CodeGen(CodeContext& context) const override;
};

class NIsZombie : public NBoolean
{
public:
    NIsZombie(NNumeric* val);
	virtual void CodeGen(CodeContext& context) const override;
private:
    NNumeric* mVal;
};


