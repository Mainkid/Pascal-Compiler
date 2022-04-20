
#pragma once
#include <memory>
#include <iostream>
#include <list>
#include <set>
#include <iterator>
#include "CToken.h"
#include <queue>
#include <stack>
#include <windows.h>
#include <WinUser.h>
#include <WinBase.h>
#include <fstream>



class TreeNode;

typedef std::shared_ptr<CToken> CTokenPtr;

enum class GenerationStage
{
	Error,
	Nothing,
	Var,
	Expression,
	ExpressionEND,
	AssignableVar,
	ExpressionLogicalEND,
	WhileSTART,
	WhileEND,
	WRITELN,
	WRITELNEND,
	IFSTART,
	ExpressionLogicalIFEND,
	ExpressionIfEnd,
	THENSTART,
	THENEND,
	ELSESTART,
	ELSEEND,
	IfEND
};

enum class IfBranch
{
	Else,
	Then,
	None
};




class IfExp
{
public:
	std::string ifCondition;
	std::string thenOperator;
	std::string elseOperator;
	int labelCount;

	IfExp(int label);

	IfExp();

};

class WhileExp
{
public:
	std::string whileCondition;
	std::string whileOperator;
	int labelCount;

	WhileExp(int label);
	WhileExp();


};

typedef std::shared_ptr<WhileExp> WhilePtr;
typedef std::shared_ptr<IfExp> IfPtr;


class CGenerator
{
private:
	GenerationStage generationStage=GenerationStage::Nothing;
	
public:
	
	CTokenPtr WriteLNToken;

	std::queue<CTokenPtr> expressionQueue; //для записи символов из анализатора

	std::queue<CTokenPtr> exprQ; //poland notation
	CTokenPtr assignedIdent; //Идентификатор присваивания
	int constCounter = 0; //Счетчик констант

	std::queue<CTokenPtr> variableQueue;
	CTokenPtr lastToken;
	std::string programHeader;
	std::string programData;
	std::string programStart;
	std::string programEnd;
	
	std::map<std::string, VariantType> varTypes;

	int label = -1; //Лейбел для цикла или для ифа

	bool isCycle = false;

	std::stack<IfPtr> ifStack;
	std::stack<WhilePtr> whileStack;

	IfBranch branch = IfBranch::None;


	CGenerator(bool isWorking);

	void WriteLN();

	void pushToken(CTokenPtr token);

	void pushToVariableQueue(CTokenPtr token);

	void setVariablesType(CIdentToken* varType);
	

	int getExprPriority(CKeywordToken* val);

	void convertToPolandNotation();
	
	//Доделать для then else
	void evaluatePolandNotation(bool isIf);
	

	void evaluateLogicalExpression(bool isIf);
	

	void evaluateIfLogicalExpression();

	void evaluateAssign();
	

	void evaluateBoolAssign();
	

	void changeGeneratorStage(GenerationStage gs);


	void printCodeToFile();

	bool isInteger(std::string str);
};