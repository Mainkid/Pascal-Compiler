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
	ENUM,
	NULLTYPE
};

enum class SemanticError
{
	NoError,
	UnknownType,
	AlreadyDeclared,
	IncompatibleTypes,
	WrongParametersValue
};


class CType;
class CIdent;
class Scope;

typedef std::shared_ptr<Scope> ScopePtr;

class CType
{
private:
	ClassType classType;
	std::set<CIdent*> constants;

public:
	CType(ClassType classTypeParam);
	void AddConstant(CIdent* curConst);

};

class CIdent
{
private:
	std::string name;
	CType* cType;
	UsageType usageType;

public:
	CIdent(std::string, UsageType, CType*);
	
};

class Scope
{
private:
	ScopePtr parent;
	std::set<CType*> typeSet;
	std::map<std::string, CIdent*> identTable;
	std::map<std::string, CType*> typeTable;
	std::map<std::string, Scope*> procTable;
	std::map<std::string, std::list<CType*>> procParam;


public:

	CType* nullType = nullptr;

	Scope(ScopePtr par);


	Scope();

	void AddParameter(CType*, std::string );

	CType* AddType(ClassType);

	CIdent* AddIdent(std::string, UsageType, CType*);
	

	CType* FindCType(std::string);

	CIdent* FindCIdent(std::string);

	SemanticError AddIdent(CIdentToken*, UsageType, CIdentToken*);

	SemanticError AddProc(CIdentToken*);



	//ѕроверить тип с текущим типом
	//ѕосмотреть, больше ли параметров или меньше
	SemanticError CheckProcedureParameters(int, CType*, std::string);

	SemanticError CheckProcParametersAmount(int, std::string);


	void AddConst(CType*, CIdent*);



	void ScopeInit();



};