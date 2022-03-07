#pragma once
#include "CToken.h"
#include <fstream>





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
	bool ReadNextLine();
	CLexer(std::string);
	~CLexer();
	bool isQuotationOpened = false;
	std::string toLowerCase(std::string);
private:
	const std::string separators = R"(:=<>;,()*+/- ")";
	const std::string unknownSymbols = "`~@¹$<>|"; 
	std::string programText;
	std::string currentToken="";
	int currentPosition=0;
	int currentLine = 0;
	std::ifstream file;
	std::string errorList="";
	
};

