#include <string>
#include <string.h>
#include <cstring>
#include <memory>
#include "CToken.h"



CToken::CToken(TokenType _type)
{
	this->tType = _type;
}

CToken::~CToken()
{

}

TokenType CToken::getType()
{
	return this->tType;
}

std::string CIdentToken::ToString()
{
	return "";
}

std::string CKeywordToken::ToString()
{
	return "";
}

std::string CConstToken::ToString()
{
	return "";
}

CIdentToken::CIdentToken() :CToken(TokenType::ttIdent)
{

}

CIdentToken::CIdentToken(std::string v) : CToken(TokenType::ttIdent)
{
	value = v;
}

CKeywordToken::CKeywordToken() : CToken(TokenType::ttKeyword)
{

}

CKeywordToken::CKeywordToken(KeyWords v) : CToken(TokenType::ttKeyword)
{
	value = v;
}

CConstToken::CConstToken(int v) : CToken(TokenType::ttConst) 
{ 
	this->value = std::make_unique<CIntVariant>(v); 
}

CConstToken::CConstToken(float v) : CToken(TokenType::ttConst)
{
	this->value = std::make_unique<CRealVariant>(v);
}

CConstToken::CConstToken(bool v) : CToken(TokenType::ttConst)
{
	this->value = std::make_unique<CBooleanVariant>(v);
}

CConstToken::CConstToken() : CToken(TokenType::ttConst)
{

}