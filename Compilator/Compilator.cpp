// Compilator.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include "CLexer.h"
#include "CToken.h"
#include <fstream>
#include <string>
#include <streambuf>
#include <memory>

using namespace std;
typedef std::unique_ptr<CToken> CTokenPtr; //fix

const string program = 
R"(while x>0.25 do
   begin
   x:=0;
   if(x>=0)then
        a=0
   end;)";

int main()
{
    
    auto lexer = new CLexer(program+' ');
    CTokenPtr token = nullptr;
    while (token = move(lexer -> GetNextToken())) {
        cout << "OK" << endl;
    }
    cout << "Hello World!\n";

    //TODO: проверить на других программах паскаль
}

