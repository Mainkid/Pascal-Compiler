#include <string>
#include <string.h>
#include <cstring>
#include <memory>
#include "CToken.h"
#include "CLexer.h"


	
	CTokenPtr CLexer::GetTokenType()
	{	
		std::string token = currentToken;
		currentToken = "";
		

		if (token=="if")
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
		

		else if (IsBoolean(token))
			return std::make_unique<CConstToken>(token=="true"); 
		else if (IsReal(token))
			return std::make_unique<CConstToken>(std::stof(token.c_str())); 
		else if (IsInteger(token))
			return std::make_unique<CConstToken>(std::stoi(token)); 
		else
			return std::make_unique<CIdentToken>(token);
		
		//TODO: добавить проверку на корректность ввода
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
			else if (currentToken != "" && separators.find(curSymbol)!= std::string::npos)
			{
				currentPosition = i;
				return GetTokenType();

				

			}
			else if ((currentToken == "<" && !(curSymbol == '=' || curSymbol == '>')) ||
				(currentToken == ">" && curSymbol != '=')||
				(currentToken == ":=")||(currentToken == "(")||(currentToken==")")||
				(currentToken == "=") ||(currentToken == "<=")||(currentToken==">="))
				 {
					currentPosition = i;
					return GetTokenType();

					
				 }
			else if (curSymbol!=' '&& curSymbol!='\n')
			{
				currentToken += curSymbol;
			}

		}



		auto token = std::make_unique<CKeywordToken>();
		return token;
	}
	CLexer :: CLexer(std::string pt) {
		programText = pt;
	};
	CLexer :: ~CLexer() {};
	


	
