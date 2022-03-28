#pragma once
#include <memory>
#include <iostream>
#include "CLexer.h"
#include "CLexer.h"
#include <list>
#include <set>
#include <iterator>

enum class UsageType
{
	TYPE,
	VAR,
	CONST,
	PROC

};

enum class ClassType
{
	SCALAR,
	ENUM
};


class CType
{
private:
	ClassType classType;
	std::set<CIdent*> constants;

public:
	CType(ClassType classTypeParam)
	{
		classType = classTypeParam;
	}

	void AddConstant(CIdent* curConst)
	{
		constants.insert(curConst);
	}

};

class CIdent
{
private:
	std::string name;
	CType* cType;
	UsageType usageType;

public:
	CIdent(std::string nameParam,UsageType usageTypeParam, CType* cTypeParam)
	{
		name = nameParam;
		usageType = usageTypeParam;
		cType = cTypeParam;
	}
};

class Scope
{
private:
	Scope* parent;
	std::set<CType*> typeSet;
	std::map<std::string, CIdent*> identTable;
	std::map<std::string, CType*> typeTable;

	CIdent* trueIdent;
	CIdent* falseIdent;

public:
	Scope(Scope* par)
	{
		parent = par;
		ScopeInit();
	}

	Scope()
	{
		ScopeInit();
	}

	CType* AddType(ClassType classType)
	{
		CType* new_type = new CType(classType);
		typeSet.insert(new_type);
		return new_type;
	}

	CIdent* AddIdent(std::string typeStr, UsageType usageType, CType* cType)
	{
		typeTable[typeStr] = cType;
		CIdent* new_ident = new CIdent(typeStr, usageType, cType);
		identTable[typeStr] = new_ident;
		return new_ident;
	}

	void AddConst(CType* type, CIdent* ident)
	{
		type->AddConstant(ident);
	}

	void ScopeInit()
	{
		AddIdent("integer", UsageType::TYPE, AddType(ClassType::SCALAR));
		AddIdent("real", UsageType::TYPE, AddType(ClassType::SCALAR));
		AddIdent("string", UsageType::TYPE, AddType(ClassType::SCALAR)); //?????????????????????
		AddIdent("boolean", UsageType::TYPE, AddType(ClassType::ENUM));



		AddConst(typeTable["boolean"], AddIdent("true", UsageType::CONST, typeTable["boolean"]));
		AddConst(typeTable["boolean"], AddIdent("false", UsageType::CONST, typeTable["boolean"]));
	}
};