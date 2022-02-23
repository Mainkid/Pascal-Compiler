#include <string>
#include <string.h>
#include <cstring>
#include <memory>
#include <iostream>
#include "CTypes.h"
#include "CToken.h"
#include "CLexer.h"




	
	CTokenPtr CLexer::GetTokenType()
	{	
		std::string token = currentToken;
		currentToken = "";
		
		if (token == "\"")
		{
			
			return std::make_unique<CKeywordToken>(KeyWords::quoteSy);
		}
		else if (isQuotationOpened)
		{
			
			return std::make_unique<CConstToken>(token);
		}


		else if (token=="if")
			return std::make_unique<CKeywordToken>(KeyWords::ifSy);
		else if (token=="else")
			return std::make_unique<CKeywordToken>(KeyWords::elseSy);
		else if (token=="then")
			return std::make_unique<CKeywordToken>(KeyWords::thenSy);
		else if (token=="while")
			return std::make_unique<CKeywordToken>(KeyWords::whileSy);
		else if (token=="begin")
			return std::make_unique<CKeywordToken>(KeyWords::beginSy);
		else if (token == "end")
			return std::make_unique<CKeywordToken>(KeyWords::endSy);
		else if (token == "do")
			return std::make_unique<CKeywordToken>(KeyWords::doSy);
		else if (token == "const")
			return std::make_unique<CKeywordToken>(KeyWords::constSy);
		else if (token == "for")
			return std::make_unique<CKeywordToken>(KeyWords::forSy);
		else if (token == "function")
			return std::make_unique<CKeywordToken>(KeyWords::functionSy);
		else if (token == "var")
			return std::make_unique<CKeywordToken>(KeyWords::varSy);
		else if (token == "to")
			return std::make_unique<CKeywordToken>(KeyWords::toSy);
		else if (token == "downto")
			return std::make_unique<CKeywordToken>(KeyWords::downToSy);
		else if (token == "procedure")
			return std::make_unique<CKeywordToken>(KeyWords::procedureSy);
		else if (token == "integer")
			return std::make_unique<CKeywordToken>(KeyWords::integerSy);
		else if (token == "string")
			return std::make_unique<CKeywordToken>(KeyWords::stringSy);
		else if (token == "real")
			return std::make_unique<CKeywordToken>(KeyWords::realSy);
		else if (token == ";")
			return std::make_unique<CKeywordToken>(KeyWords::semicolonSy);
		else if (token == "repeat")
			return std::make_unique<CKeywordToken>(KeyWords::repeatSy);
		else if (token == "until")
			return std::make_unique<CKeywordToken>(KeyWords::untilSy);
		else if (token == "boolean")
			return std::make_unique<CKeywordToken>(KeyWords::booleanSy);
		else if (token == "case")
			return std::make_unique<CKeywordToken>(KeyWords::caseSy);
		else if (token == "of")
			return std::make_unique<CKeywordToken>(KeyWords::ofSy);
		else if (token == "+")
			return std::make_unique<CKeywordToken>(KeyWords::plusSy);
		else if (token == "-")
			return std::make_unique<CKeywordToken>(KeyWords::minusSy);
		else if (token == "*")
			return std::make_unique<CKeywordToken>(KeyWords::multiplySy);
		else if (token == "/")
			return std::make_unique<CKeywordToken>(KeyWords::divisionSy);
		else if (token == "=")
			return std::make_unique<CKeywordToken>(KeyWords::equalSy);
		else if (token == "<")
			return std::make_unique<CKeywordToken>(KeyWords::lessSy);
		else if (token == "<=")
			return std::make_unique<CKeywordToken>(KeyWords::lessEqualSy);
		else if (token == ">")
			return std::make_unique<CKeywordToken>(KeyWords::moreSy);
		else if (token == ">=")
			return std::make_unique<CKeywordToken>(KeyWords::moreEqualSy);
		else if (token == "<>")
			return std::make_unique<CKeywordToken>(KeyWords::notEqualSy);
		else if (token == ":=")
			return std::make_unique<CKeywordToken>(KeyWords::assignSy);
		else if (token == "and")
			return std::make_unique<CKeywordToken>(KeyWords::andSy);
		else if (token == "or")
			return std::make_unique<CKeywordToken>(KeyWords::orSy);
		else if (token == "not")
			return std::make_unique<CKeywordToken>(KeyWords::notSy);
		else if (token == "(")
			return std::make_unique<CKeywordToken>(KeyWords::bracketOpenSy);
		else if (token == ")")
			return std::make_unique<CKeywordToken>(KeyWords::bracketCloseSy);
		else if (token == ":")
			return std::make_unique<CKeywordToken>(KeyWords::colonSy);
		else if (token == "program")
			return std::make_unique<CKeywordToken>(KeyWords::programSy);
		else if (token == "type")
			return std::make_unique<CKeywordToken>(KeyWords::typeSy);
		else if (token == ",")
			return std::make_unique<CKeywordToken>(KeyWords::commaSy);
		else if (token =="in")
			return std::make_unique<CKeywordToken>(KeyWords::inSy);
		else if (token == "div")
			return std::make_unique<CKeywordToken>(KeyWords::divSy);
		else if (token == "mod")
			return std::make_unique<CKeywordToken>(KeyWords::modSy);
		

		else if (IsBoolean(token))
			return std::make_unique<CConstToken>(token=="true"); 
		else if (IsReal(token))
			return std::make_unique<CConstToken>(std::stof(token.c_str())); 
		else if (IsInteger(token))
			return std::make_unique<CConstToken>(std::stoi(token)); 
		else
		{
			LexicalError errType = IsIdentifier(token);

			if (errType == LexicalError::noError)
				return std::make_unique<CIdentToken>(token);
			else if (errType == LexicalError::incorrectIdentifier)
				std::cout << "Некорректный идентификатор " << std::endl;
			else if (errType == LexicalError::incorrectRealValue)
				std::cout << "Некорректное значение вещественного типа " << std::endl;
			else if (errType == LexicalError::unknownCharacter)
				std::cout << "Неизвестный символ" << std::endl;

			return GetNextToken();
		}
			
	}

	LexicalError CLexer::IsIdentifier(std::string token)
	{
		if (token[0] >= '0' && token[0] <= '9')
			for (int i = 1; i < token.length(); i++)
				if (isalpha(token[i]))
					return LexicalError::incorrectIdentifier;

		int dotCounter = 0;
		for (int i = 0; i < token.length(); i++)
		{
			if (token[i] == '.')
				dotCounter++;
		}
		if (dotCounter >= 2)
			return LexicalError::incorrectRealValue;

		for (int i = 0; i < token.length(); i++)
		{
			if (unknownSymbols.find(token[i])!= std::string::npos)
				return LexicalError::unknownCharacter;
		}
			
		return LexicalError::noError;
	}

	bool CLexer::IsReal(std::string token)
	{
		bool hasDot = false;

		for (int i = 0; i < token.length(); i++)
		{
			if ((token[i] < '0' || token[i]>'9')&&token[i]!='.')
				return false;
			else if (token[i] == '.')
				if (!hasDot)
					hasDot = true;
				else
					return false;
		}

		return hasDot;
	}
	
	bool CLexer::IsInteger(std::string token)
	{
		for (int i = 0; i < token.length(); i++)
		{
			if (token[i] < '0' || token[i]>'9')
				return false;
		}
	}

	bool CLexer::IsBoolean(std::string token)
	{
		return token == "true" || token == "false";
	}

	CTokenPtr CLexer::GetNextToken() 
	{
		if (currentPosition == programText.length() - 1)
			return nullptr;
		for (int i = currentPosition; i < programText.length(); i++)
		{
			
				

			char curSymbol = programText[i];
			if (isQuotationOpened && curSymbol != '\"')
			{
				currentToken += curSymbol;
			}
			else if (curSymbol == '\"')
			{
				
				if (currentToken == "")
				{
					isQuotationOpened = !isQuotationOpened;
					currentPosition = i + 1;
					currentToken += curSymbol;
					return GetTokenType();
				}
				else
				{
					currentPosition = i;
					return GetTokenType();
				}
			}
			else if (!isQuotationOpened)
			{

				if (currentToken == ":" && curSymbol == '=')
				{
					currentToken += curSymbol;
				}
				else if (currentToken == "<" && curSymbol == '=')
				{
					currentToken += curSymbol;
				}
				else if (currentToken == ">" && curSymbol == '=')
				{
					currentToken += curSymbol;
				}
				else if (currentToken == "<" && curSymbol == '>')
				{
					currentToken += curSymbol;
				}
				else if (currentToken != "" && separators.find(curSymbol) != std::string::npos)
				{
					currentPosition = i;
					return GetTokenType();



				}
				else if ((currentToken == "<" && !(curSymbol == '=' || curSymbol == '>')) ||
					(currentToken == ">" && curSymbol != '=') ||
					(currentToken == ":=") || (currentToken == "(") || (currentToken == ")") ||
					(currentToken == "=") || (currentToken == "<=") || (currentToken == ">=")||
					(currentToken ==":" && curSymbol !='='))
				{
					currentPosition = i;
					return GetTokenType();


				}
				else if (curSymbol != ' ' && curSymbol != '\n' || currentToken == "\"")
				{
					currentToken += curSymbol;
					if (currentToken == ","|| currentToken == ";"||currentToken=="begin"||currentToken=="end")
					{
						currentPosition = i + 1;
						return GetTokenType();
					}
					
				}
			}

		}



		auto token = std::make_unique<CKeywordToken>();
		return token;
	}
	CLexer :: CLexer(std::string pt) {
		programText = pt;
	};
	CLexer :: ~CLexer() {};
	


	
