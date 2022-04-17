// Compilator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>
#include "CLexer.h" 
#include "CToken.h"
#include "CVariant.h"
#include "CSyntax.h"

#include <streambuf>
#include <memory>

using namespace std;

//TODO LEXER: Дописать парсинг комментариев
//TODO LEXER: Регистр строки
//TODO SYNTAX: Добавить WriteLN





string program = 
R"(program myProgram; 
33 true false 14.12
end.)";

void testLexer()
{
    auto lexer = new CLexer(program + ' ');
    CTokenPtr token = nullptr;
    while (token = move(lexer->GetNextToken())) {

        if (token.get()->getType() == TokenType::ttKeyword)
        {
            int code= static_cast<int>(dynamic_cast<CKeywordToken*>(token.get())->GetValue());
            cout << code <<" ";
        }
        else if (token.get()->getType() == TokenType::ttConst)
        {

            cout << "constVal(" << dynamic_cast<CConstToken*>(token.get())->ToString() <<") ";
        }
        else if (token.get()->getType() == TokenType::ttIdent)
        {
            cout << "ident(" << dynamic_cast<CIdentToken*>(token.get())->GetValue() << ") ";
        }

        
    }

}

int main()
{
    
    //setlocale(0, "");
    std::setlocale(LC_NUMERIC, "POSIX");
    //testLexer();
    

    auto syntax = new CSyntax();
    syntax->StartSyntaxAnalyze(program);

}



