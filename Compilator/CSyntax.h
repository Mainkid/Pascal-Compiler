#pragma once
#include <memory>
#include <iostream>
#include "CLexer.h"
#include "CLexer.h"
#include "CSemantic.h"
#include <list>
#include <set>
#include <iterator>
#include <queue>

typedef std::unique_ptr<CToken> CTokenPtr;

struct followers
{
	std::set<KeyWords> l;
};


enum class Constructions
{
	no
};


class CSyntax
{

public:
	void StartSyntaxAnalyze(std::string);
	
	CSyntax();
	~CSyntax();
private:

	bool isSkipping;
	int semErrPos=-1;
	int semErrLine=-1;


	KeyWords skipToKeyWord;
	Scope* currentScope;
	std::queue <CIdentToken*> identQueue;

	void AddNewVariables(CIdentToken*,UsageType);
	void ThrowSemanticError(SemanticError);
	CType* GetCType(CIdentToken*);
	CType* CheckSemantic(CType* t1, CType* t2);
	CType* CheckAssignSemantic(CType*, CType*);

	void skipToNextKeyword(KeyWords, std::set<KeyWords>);
	void Program();
	CIdentToken* AcceptIdent(followers, std::set<KeyWords>);
	void AcceptKeyword(KeyWords, followers,std::set<KeyWords>);
	void AcceptKeyword();
	CType* AcceptConst(followers, std::set<KeyWords>);
	bool CheckConst();
	bool CheckKeyword(KeyWords);
	bool CheckIdent();
	void SkipToKeyWord(KeyWords);
	void GetNextNotEmptyToken();
	std::string GetConstType(std::string);
	
	void PrintError(bool,int, int);
	void PrintError(KeyWords, int, int);
	void PrintError(Constructions, int, int);

	void Block(followers);
	void TypeSection(followers);
	void TypeDefinition(followers);
	void VariableSection(followers);
	void OneTypeVariableDefinition(followers);
	void AcceptTypeKeywords();
	void ProcedureSection(followers);
	void FactParameter(followers);

	void OperatorSection(followers);
	void ComposeOperator(followers);
	void Operator(followers);
	void UnmarkedOperator(followers);
	void SimpleOperator(followers);
	void ComplexOperator(followers);
	CType* AssignOperator(followers);
	void ProcedureOperator(followers);
	CType* Expression(followers);
	CType* SimpleExpression(followers);
	CType* Term(followers);
	CType* Multiplier(followers);

	void ChoosingOperator(followers);
	void CycleOperator(followers);
	void IfOperator(followers); //!
	void ChoiseOperator(followers);
	
	CType* VariantListElement(followers);
	CType* ListVariantLabels(followers);

	void ProcedureDescription(followers);
	void ProcedureHeader(followers);
	void FormalOperatorsSection(followers);
	void ParametersGroup(followers);

	
	CTokenPtr currentToken=nullptr;
	CLexer* lexer=nullptr;

};



