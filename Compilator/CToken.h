#pragma once
#include <string.h>
#include <string>
#include <map>
#include "CVariant.h"
#include "CToken.h"



typedef std::unique_ptr<CVariant> CVariantPtr;

enum class TokenType {
	ttIdent,
	ttKeyword,
	ttConst
};

enum class KeyWords {
	errorValueSy,
	emptyValueSy,
	quoteSy,
	ifSy,
	elseSy,
	thenSy,
	whileSy,
	beginSy,
	endSy,
	doSy,
	constSy,
	forSy,
	functionSy,
	varSy,
	toSy,
	downToSy,
	procedureSy,
	integerSy,
	stringSy,
	realSy,
	booleanSy,
	caseSy,
	ofSy,
	repeatSy,
	untilSy,
	semicolonSy,
	programSy,
	typeSy,
	commaSy,
	inSy,
	divSy,
	modSy,
	dotSy,

	plusSy,
	minusSy,
	multiplySy,
	divisionSy,
	equalSy,
	lessSy,
	lessEqualSy,
	moreSy,
	moreEqualSy,
	notEqualSy,
	assignSy,
	andSy,
	orSy,
	notSy,
	bracketOpenSy,
	bracketCloseSy,
	colonSy,
	writeLN,

	errIdent,
	errConst
};



enum class ConstType
{
	Integer,
	String,
	Boolean,
	Real

};

class CToken {
private:
	TokenType tType;
public:
	CToken(TokenType _type);
	~CToken();
	TokenType getType();
	virtual std::string ToString()=0;
};

class CIdentToken : public CToken {
public:
	std::string ToString() override;
	CIdentToken();
	CIdentToken(std::string);
	std::string GetValue();
private:
	std::string value;
};
class CKeywordToken : public CToken {
public:
	std::string ToString() override;
	CKeywordToken();
	CKeywordToken(KeyWords v);
	KeyWords GetValue();
private:
	KeyWords value=KeyWords::emptyValueSy;
};
class CConstToken : public CToken {
public:
	std::string ToString() override;
	CConstToken();
	CConstToken(int v);
	CConstToken(float v);
	CConstToken(bool v);
	CConstToken(std::string v);
	VariantType GetConstType();


private:
	CVariantPtr value;
	
};