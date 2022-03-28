#pragma once
#include <memory>
#include <iostream>
#include "CLexer.h"
#include "CLexer.h"
#include <list>
#include <set>
#include <iterator>

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
	KeyWords skipToKeyWord;

	void skipToNextKeyword(KeyWords, std::set<KeyWords>);
	void Program();
	void AcceptIdent(followers, std::set<KeyWords>);
	void AcceptKeyword(KeyWords, followers,std::set<KeyWords>);
	void AcceptKeyword();
	void AcceptConst(followers, std::set<KeyWords>);
	bool CheckConst();
	bool CheckKeyword(KeyWords);
	bool CheckIdent();
	void SkipToKeyWord(KeyWords);
	void GetNextNotEmptyToken();
	
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
	void AssignOperator(followers);
	void ProcedureOperator(followers);
	void Expression(followers);
	void SimpleExpression(followers);
	void Term(followers);
	void Multiplier(followers);

	void ChoosingOperator(followers);
	void CycleOperator(followers);
	void IfOperator(followers); //!
	void ChoiseOperator(followers);
	
	void VariantListElement(followers);
	void ListVariantLabels(followers);

	void ProcedureDescription(followers);
	void ProcedureHeader(followers);
	void FormalOperatorsSection(followers);
	void ParametersGroup(followers);

	
	CTokenPtr currentToken=nullptr;
	CLexer* lexer=nullptr;

};



