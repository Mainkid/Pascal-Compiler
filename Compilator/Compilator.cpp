﻿// Compilator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
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
//TODO: Дописать парсинг комментариев
//TODO: Вывести все typedef в отдельный хеддер
//Для всех токенов сделать метод для получения значений
//TODO SYNTAX: Секция типов доделать
//TODO SYNTAX: Секция



string program = 
R"(program myProgram; 
33 true false 14.12 fucking 
end.)";

void testLexer()
{
    auto lexer = new CLexer(program + ' ');
    CTokenPtr token = nullptr;
    while (token = move(lexer->GetNextToken())) {

        if (token.get()->getType() == TokenType::ttKeyword)
        {
            int code= static_cast<int>(dynamic_cast<CKeywordToken*>(token.release())->GetValue());
            cout << code <<" ";
        }
        else if (token.get()->getType() == TokenType::ttConst)
        {

            cout << "constVal(" << dynamic_cast<CConstToken*>(token.release())->ToString() <<") ";
        }
        else if (token.get()->getType() == TokenType::ttIdent)
        {
            cout << "ident(" << dynamic_cast<CIdentToken*>(token.release())->GetValue() << ") ";
        }

        
    }

}

int main()
{
    
    setlocale(0, "");
    testLexer();
    

    //auto syntax = new CSyntax();
    //syntax->StartSyntaxAnalyze(program);

}



