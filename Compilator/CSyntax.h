#pragma once
#include <memory>
#include <iostream>
#include "CLexer.h"
#include "CLexer.h"
#include "CSemantic.h"
#include "CGenerator.h"
#include <list>
#include <set>
#include <iterator>
#include <queue>

typedef std::shared_ptr<CToken> CTokenPtr;
typedef std::unique_ptr<CLexer> CLexerPtr;


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
	ScopePtr currentScope;
	std::queue <CTokenPtr> identQueue;

	void AddNewProcIdent(CIdentToken*);
	void AddNewVariables(CIdentToken*,UsageType);
	bool ThrowSemanticError(SemanticError);
	CType* GetCType(CIdentToken*);
	CType* CheckSemantic(CType* t1, CType* t2);
	CType* CheckAssignSemantic(CType*, CType*);

	
	void skipToNextKeyword(KeyWords, std::set<KeyWords>);
	void Program();
	CTokenPtr AcceptIdent(followers, std::set<KeyWords>);
	CKeywordToken* AcceptKeyword(KeyWords, followers,std::set<KeyWords>);
	CKeywordToken* AcceptKeyword();
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
	CIdentToken* AcceptTypeKeywords();
	void ProcedureSection(followers);
	void FactParameter(followers,std::string);

	void OperatorSection(followers);
	void ComposeOperator(followers);
	void Operator(followers);
	void UnmarkedOperator(followers);
	void SimpleOperator(followers);
	void ComplexOperator(followers);
	CType* AssignOperator(followers);
	void ProcedureOperator(followers, std::string);
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
	void FormalOperatorsSection(followers,std::string);
	void ParametersGroup(followers,std::string);
	void ParametersGroup(followers);
	

	void AddNewVariablesToBrackets(CIdentToken*, std::string);
	CTokenPtr currentToken=nullptr;
	CLexerPtr lexer=nullptr;
	std::unique_ptr<CGenerator> generator = nullptr;

	CTokenPtr lastToken = nullptr;
};



