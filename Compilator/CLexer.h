#pragma once
#include "CToken.h"

typedef std::unique_ptr<CToken> CTokenPtr; // fix





class CLexer
{
public:
	CTokenPtr GetTokenType();
	CTokenPtr GetNextToken();
	bool IsReal(std::string);
	bool IsInteger(std::string);
	bool IsBoolean(std::string);
	CLexer(std::string);
	~CLexer();
private:
	const std::string separators = ":=<>;,()*+/- ";
	std::string programText;
	std::string currentToken="";
	int currentPosition=0;
};

