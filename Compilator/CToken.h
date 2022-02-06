#pragma once
#include <string.h>
#include <string>
#include <map>
#include "CVariant.h"

typedef std::unique_ptr<CVariant> CVariantPtr;

enum class TokenType {
	ttIdent,
	ttKeyword,
	ttConst
};

enum class KeyWords {
	emptyValueSy,
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
	bracketCloseSy
};



class CToken {
private:
	TokenType tType;
public:
	CToken(TokenType _type);
	~CToken();
	TokenType getType();
	virtual std::string ToString() = 0;
};

class CIdentToken : public CToken {
public:
	std::string ToString() override;
	CIdentToken();
	CIdentToken(std::string);
	std::string value;
};
class CKeywordToken : public CToken {
public:
	std::string ToString() override;
	CKeywordToken();
	CKeywordToken(KeyWords v);
	KeyWords value=KeyWords::emptyValueSy;
};
class CConstToken : public CToken {
public:
	std::string ToString() override;
	CConstToken();
	CConstToken(int v);
	CConstToken(float v);
	CConstToken(bool v);
private:
	CVariantPtr value;
};