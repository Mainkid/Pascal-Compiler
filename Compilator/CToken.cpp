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
	return value;
}

std::string CIdentToken::GetValue()
{
	return value;
}


std::string CKeywordToken::ToString()
{
	//Ok?
	return "KeyWord";
}

std::string CConstToken::ToString()
{
	if (value.get()->GetVariantType() == VariantType::vtBoolean)
	{
		return dynamic_cast<CBooleanVariant*>(value.get())->ToString();
	}
	else if (value.get()->GetVariantType() == VariantType::vtInt)
	{
		return dynamic_cast<CIntVariant*>(value.get())->ToString();
	}
	else if (value.get()->GetVariantType() == VariantType::vtReal)
	{
		return dynamic_cast<CRealVariant*>(value.get())->ToString();
	}
	else if (value.get()->GetVariantType() == VariantType::vtString)
	{
		return dynamic_cast<CStringVariant*>(value.get())->ToString();
	}

	return "";
}

VariantType CConstToken::GetConstType()
{
	return value.get()->GetVariantType();
	
}

KeyWords CKeywordToken::GetValue()
{
	return value;
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

CConstToken::CConstToken(std::string v) : CToken(TokenType::ttConst)
{
	this->value = std::make_unique<CStringVariant>(v);
}

CConstToken::CConstToken() : CToken(TokenType::ttConst)
{

}
