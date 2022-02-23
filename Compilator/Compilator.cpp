// Compilator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "CLexer.h"
#include "CToken.h"
#include "CVariant.h"
#include "CSyntax.h"
#include <fstream>
#include <string>
#include <streambuf>
#include <memory>

using namespace std;
//TODO: Дописать парсинг комментариев
//TODO: Вывести все typedef в отдельный хеддер
//Для всех токенов сделать метод для получения значений
//TODO SYNTAX: Секция типов доделать
//TODO SYNTAX: Секция



const string program = 
R"(program myProgram;
begin
p1:=n div 1000;
p4:=n mod 10;
p2:=(n div 100) mod 10;
p3:=(n div 10) mod 10;
end.)";

int main()
{
    
    /*auto lexer = new CLexer(program+' ');
    CTokenPtr token = nullptr;
    while (token = move(lexer -> GetNextToken())) {
        
        cout << "OK" << endl;
    }
    cout << "Hello World!\n";*/

    auto syntax = new CSyntax();
    syntax->StartSyntaxAnalyze(program);

}

