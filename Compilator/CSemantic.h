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

enum class SemanticError
{
	NoError,
	UnknownType,
	AlreadyDeclared
};

class CType;
class CIdent;
class Scope;

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
	std::map<std::string, Scope*> procTable;


public:
	Scope(Scope* par)
	{
		parent = par;
		//ScopeInit();
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

	CType* FindCType(std::string typeStr)
	{
		Scope* currentScope = this;
		while (currentScope != NULL)
		{
			if (currentScope->typeTable.count(typeStr))
				return currentScope->typeTable[typeStr];
			else
				currentScope = currentScope->parent;
		}
		return NULL;
	}

	SemanticError AddIdent(CIdentToken* ident, UsageType usageType, CIdentToken* typeStr)
	{
		CType* cType = FindCType(typeStr->GetValue());
		if (!cType)
			return SemanticError::UnknownType;
		if (identTable.count(ident->GetValue())>0)
			return SemanticError::AlreadyDeclared;
		AddIdent(ident->GetValue(), usageType, cType);
		return SemanticError::NoError;
	}

	Scope* AddProc(CIdentToken* ident)
	{
		AddIdent(ident->GetValue(), UsageType::PROC, NULL);
		procTable[ident->GetValue()] = new Scope();
		procTable[ident->GetValue()]->parent = this;
	}
	
	

	void AddConst(CType* type, CIdent* ident)
	{
		type->AddConstant(ident);
	}

	void ScopeInit()
	{
		parent = NULL;
		AddIdent("integer", UsageType::TYPE, AddType(ClassType::SCALAR));
		AddIdent("real", UsageType::TYPE, AddType(ClassType::SCALAR));
		AddIdent("string", UsageType::TYPE, AddType(ClassType::SCALAR)); //?????????????????????
		AddIdent("boolean", UsageType::TYPE, AddType(ClassType::ENUM));



		AddConst(typeTable["boolean"], AddIdent("true", UsageType::CONST, typeTable["boolean"]));
		AddConst(typeTable["boolean"], AddIdent("false", UsageType::CONST, typeTable["boolean"]));
	}
};