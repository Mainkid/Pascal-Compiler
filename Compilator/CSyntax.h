#pragma once
#include <memory>
#include <iostream>
#include "CLexer.h"

typedef std::unique_ptr<CToken> CTokenPtr;

class CSyntax
{

public:
	void StartSyntaxAnalyze(std::string);
	
	CSyntax();
	~CSyntax();
private:
	void Program();
	void AcceptIdent();
	void AcceptKeyword(KeyWords);
	void AcceptKeyword();
	void AcceptConst();
	bool CheckConst();
	bool CheckKeyword(KeyWords);
	bool CheckIdent();
	

	void Block();
	void TypeSection();
	bool TypeDefinition(bool);
	void VariableSection();
	bool OneTypeVariableDefinition(bool);
	void AcceptTypeKeywords();
	void ProcedureSection();

	void OperatorSection();
	bool ComposeOperator(bool);
	void Operator();
	void UnmarkedOperator();
	bool SimpleOperator(bool);
	bool ComplexOperator(bool);
	bool AssignOperator(bool);
	bool ProcedureOperator(bool);
	bool Expression();
	bool SimpleExpression();
	bool Term();
	bool Multiplier();

	bool ChoosingOperator(bool);
	bool CycleOperator(bool);
	bool IfOperator(bool);
	bool ChoiseOperator(bool);

	
	CTokenPtr currentToken=nullptr;
	CLexer* lexer=nullptr;

};



