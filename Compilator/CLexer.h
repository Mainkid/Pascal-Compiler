#pragma once
#include "CToken.h"






typedef std::unique_ptr<CToken> CTokenPtr;


enum class LexicalError
{
	noError,
	incorrectIdentifier,
	incorrectRealValue,
	unknownCharacter,
	constValueOverflow
};


class CLexer
{
public:
	CTokenPtr GetTokenType();
	CTokenPtr GetNextToken();
	bool IsReal(std::string);
	bool IsInteger(std::string);
	bool IsBoolean(std::string);
	LexicalError IsIdentifier(std::string);
	CLexer(std::string);
	~CLexer();
	bool isQuotationOpened = false;
private:
	const std::string separators = R"(:=<>;,()*+/- ")";
	const std::string unknownSymbols = "`~@¹$<>|"; 
	std::string programText;
	std::string currentToken="";
	int currentPosition=0;
	
};

