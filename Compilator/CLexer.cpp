
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <memory>




#include "CTypes.h"
#include "CToken.h"
#include "CLexer.h"




	
	CTokenPtr CLexer::GetTokenType()
	{	
		std::string token = toLowerCase(currentToken);
		currentToken = "";

		if (currentPosition >= programText.length() - 1)
		{
			if (ReadNextLine())
				std::cout << std::endl;
		}
		
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
		else if (token == ".")
			return std::make_unique<CKeywordToken>(KeyWords::dotSy);
		

		else if (IsBoolean(token))
			return std::make_unique<CConstToken>(token=="true");
		else if (IsInteger(token))
		{
		if (std::stoi(token) > -32678 && std::stoi(token) < 32767)
			return std::make_unique<CConstToken>(std::stoi(token));
		else
		{
			errorList += "Переполнение Integer (Строка: " + std::to_string(currentLine+1) + ", Позиция: " + std::to_string(currentPosition+1-token.length()) + ")\n";
			return std::make_unique<CKeywordToken>(KeyWords::errorValueSy);
		}
		}
		else if (IsReal(token))
		{
			if (std::stof(token.c_str())> 2.9e-39&& std::stof(token.c_str())< 1.7e38)
				return std::make_unique<CConstToken>(std::stof(token.c_str()));
			else
			{
				errorList += "Переполнение Real (Строка: " + std::to_string(currentLine+1) + ", Позиция: " + std::to_string(currentPosition+1 - token.length()) + ")\n";
				return std::make_unique<CKeywordToken>(KeyWords::errorValueSy);
			}
		}
		
			
		else
		{
			LexicalError errType = IsIdentifier(token);

			if (errType == LexicalError::noError)
				return std::make_unique<CIdentToken>(token);
			else if (errType == LexicalError::incorrectIdentifier)
			{
				errorList += "Некорректный идентификатор (Строка: " + std::to_string(currentLine+1) + ", Позиция: " + std::to_string(currentPosition+1 - token.length()) + ")\n";
				return std::make_unique<CKeywordToken>(KeyWords::errorValueSy);
			}
			else if (errType == LexicalError::incorrectRealValue)
			{
				errorList += "Некорректное значение вещественного типа (Строка: " + std::to_string(currentLine+1) + ", Позиция: " + std::to_string(currentPosition+1 - token.length()) + ")\n";
				return std::make_unique<CKeywordToken>(KeyWords::errorValueSy);
			}
			else if (errType == LexicalError::unknownCharacter)
			{
				errorList += "Неизвестный символ (Строка: " + std::to_string(currentLine+1) + ", Позиция: " + std::to_string(currentPosition+1 - token.length()) + ")\n";
				return std::make_unique<CKeywordToken>(KeyWords::errorValueSy);
			}
			return GetNextToken();
		}
			
	}

	LexicalError CLexer::IsIdentifier(std::string token)
	{
		if (token[0] >= '0' && token[0] <= '9')
			return LexicalError::incorrectIdentifier;
			/*for (int i = 1; i < token.length(); i++)
				if (isalpha(token[i]))
					return LexicalError::incorrectIdentifier;*/

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

		if (token.length() >= 2 && token[0] == '0' && token[1]>= '0'&&token[1]<='9')
			return false;

		if (IsInteger(token))
			return true;

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
		if (token.length() >= 2 && token[0] == '0' && token[1] >= '0' && token[1] <= '9')
			return false;

		for (int i = 0; i < token.length(); i++)
		{
			if (token[i] < '0' || token[i]>'9')
				return false;
		}



		return true;
	}

	bool CLexer::IsBoolean(std::string token)
	{
		return token == "true" || token == "false";
	}


	CTokenPtr CLexer::GetNextToken() 
	{
		//if (currentPosition == programText.length() - 1)
			//return nullptr;
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
				else if (curSymbol != ' ' && curSymbol != '\n' && curSymbol != '\t' || currentToken == "\"")
				{
					currentToken += curSymbol;
					if (currentToken == ","|| currentToken == ";"|| toLowerCase(currentToken) =="begin"||
						toLowerCase(currentToken)=="end"||currentToken=="."||currentToken=="+"||currentToken=="-"||
						currentToken=="*"|| currentToken=="/")
					{

						currentPosition = i + 1;
						return GetTokenType();
					}
					
				}
			}

		}



		auto token = std::make_unique<CKeywordToken>();
		if (ReadNextLine())
			return token;
		else
		{
			std::cout << std::endl;
			std::cout << errorList << std::endl;

			if (isQuotationOpened)
				std::cout << "Кавычки не закрыты" << std::endl;

			return nullptr;
		}
	}

	std::string CLexer::toLowerCase(std::string token)
	{
		for (int i = 0; i < token.length(); i++)
		{
			token[i] = std::tolower(token[i]);
		}
		return token;
	}

	bool CLexer::ReadNextLine()
	{
		
		if (!getline(file, programText))
		 return false;
		
		programText += " ";
		currentPosition = 0;
		currentLine++;
		return true;
	}

	CLexer :: CLexer(std::string path) {
		
        file.open("C:\\Users\\jdczy\\source\\repos\\Compilator\\program1.pas");
		getline(file, programText);
		currentPosition = 0;
		currentLine = 0;
		programText += " ";
			//std::cout << s << std::endl; // выводим на экран
			//s += "+"; // что нибудь делаем со строкой например я добавляю плюсик в конце каждой строки
			//stdcout << s << endl; // и снова вывожу на экран но уже модифицированную строку (без записи ее в файл)

		
	};
	CLexer :: ~CLexer() {
		file.close();
	};
	


	
