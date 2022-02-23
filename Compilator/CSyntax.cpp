#include "CSyntax.h"
#include "CLexer.h"


CSyntax::CSyntax()
{

}

CSyntax::~CSyntax()
{

}

void CSyntax::StartSyntaxAnalyze(std::string program)
{
    lexer = new CLexer(program + ' ');
    currentToken = move(lexer->GetNextToken());
    Program();
    /*while (currentToken = move(lexer->GetNextToken())) {
        AcceptKeyword(KeyWords::elseSy);
        std::cout << "OK" << std::endl;
    }*/

}

void CSyntax::AcceptKeyword()
{
    std::cout << "ACCEPTED" << std::endl;
    currentToken = move(lexer->GetNextToken());
}

void CSyntax::AcceptKeyword(KeyWords keyWord)
{
    //Проверка на то, что токен является ключевым словом
    //CKeywordToken* keyWordToken = dynamic_cast<CKeywordToken*>(currentToken.get());
    
    if (currentToken.get()->getType()!=TokenType::ttKeyword)
    {
        std::cout << "Ожидалось ключевое слово со значением... "<< std::endl;
    }
    else
    {
        if (dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == keyWord)
            std::cout << "AIGHT" << std::endl;
        else
            std::cout << "ERROR";
    }
    currentToken = move(lexer->GetNextToken());


}

void CSyntax::AcceptIdent()
{
    //CIdentToken* identToken = dynamic_cast<CIdentToken*>(currentToken.get());
    if (currentToken.get()->getType() != TokenType::ttIdent)
    {
        std::cout << "Ожидался идентификатор..." << std::endl;
    }
    else
    {
        std::cout << "AIGHT" << std::endl;
    }
    currentToken = move(lexer->GetNextToken());


}

void CSyntax::AcceptTypeKeywords()
{
    if (currentToken.get()->getType() != TokenType::ttKeyword)
    {
        std::cout << "Ожидалось ключевое слово со значением... " << std::endl;
    }
    else
    {
        KeyWords currentKeyword = dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue();
        if (currentKeyword == KeyWords::booleanSy ||currentKeyword == KeyWords::stringSy || currentKeyword == KeyWords::integerSy || currentKeyword == KeyWords::realSy)
            std::cout << "AIGHT" << std::endl;
        else
            std::cout << "ERROR";
    }
    currentToken = move(lexer->GetNextToken());
}

void CSyntax::AcceptConst()
{
    if (currentToken.get()->getType() != TokenType::ttConst)
    {
        std::cout << "Ожидалась константа..." << std::endl;
    }
    else
    {
        std::cout << "AIGHT" << std::endl;
    }
    currentToken = move(lexer->GetNextToken());
}

bool CSyntax::CheckConst()
{
    return (currentToken.get()->getType() == TokenType::ttConst);
}

bool CSyntax::CheckKeyword(KeyWords keyWord)
{
    if (currentToken.get()->getType() != TokenType::ttKeyword)
    {
        return false;
    }
    else
    {
        CKeywordToken* keyWordToken = dynamic_cast<CKeywordToken*>(currentToken.get());
        if (keyWordToken->GetValue() == keyWord)
            return true;
        else 
            return false;
    }

}

bool CSyntax::CheckIdent()
{
    if (currentToken.get()->getType() != TokenType::ttIdent)
    {
        return false;
    }
    else
    {
        CIdentToken* keyWordToken = dynamic_cast<CIdentToken*>(currentToken.get());
        if (keyWordToken)
            return true;
        else
            return false;
    }
}

void CSyntax::Program()
{
    AcceptKeyword(KeyWords::programSy);
    AcceptIdent();
    AcceptKeyword(KeyWords::semicolonSy);
    Block();
    
}

void CSyntax::Block()
{
    TypeSection();
    VariableSection();
    ProcedureSection();
    OperatorSection();
}

void CSyntax::TypeSection()
{
    if (CheckKeyword(KeyWords::typeSy))
    {
        AcceptKeyword(KeyWords::typeSy);
        TypeDefinition(false);
        AcceptKeyword(KeyWords::semicolonSy);
        while (TypeDefinition(true))
        {
            AcceptKeyword(KeyWords::semicolonSy);
        }
    }
    
}

bool CSyntax::TypeDefinition(bool canBeSkipped)
{
    if (canBeSkipped && !CheckIdent())
        return false;
    AcceptIdent();
    AcceptKeyword(KeyWords::equalSy);
    //Type(); //Доделать
    return true;
}

void CSyntax::VariableSection()
{
    if (CheckKeyword(KeyWords::varSy))
    {
        AcceptKeyword(KeyWords::varSy);
        OneTypeVariableDefinition(false);
        AcceptKeyword(KeyWords::semicolonSy);
        while (OneTypeVariableDefinition(true))
        {
            AcceptKeyword(KeyWords::semicolonSy);
        }
    }
}

bool CSyntax::OneTypeVariableDefinition(bool canBeSkipped)
{
    if (canBeSkipped && !CheckIdent())
        return false;
    AcceptIdent();
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword(KeyWords::commaSy);
        AcceptIdent();
    }
    AcceptKeyword(KeyWords::colonSy);
    AcceptTypeKeywords();
        return true;
}

void CSyntax::ProcedureSection()
{

}

void CSyntax::OperatorSection()
{
    ComposeOperator(false);
}

bool CSyntax::ComposeOperator(bool isChecking)
{
    if (isChecking)
        return CheckKeyword(KeyWords::beginSy);


    AcceptKeyword(KeyWords::beginSy);
    Operator();
    while (CheckKeyword(KeyWords::semicolonSy))
    {
        AcceptKeyword(KeyWords::semicolonSy);
        Operator();
    }
    AcceptKeyword(KeyWords::endSy);
    return true;
}


void CSyntax::Operator()
{
    UnmarkedOperator();
}

void CSyntax::UnmarkedOperator()
{
    if (SimpleOperator(true))
        SimpleOperator(false);
    else
        ComplexOperator(false);
}

bool CSyntax::SimpleOperator(bool isChecking)
{
    if (isChecking)
        return (AssignOperator(true) || ProcedureOperator(true));

    if (AssignOperator(true))
        AssignOperator(false);
    else
        ProcedureOperator(false);

    return true;


    
}

bool CSyntax::AssignOperator(bool isChecking)
{
    if (isChecking)
        return CheckIdent();

    AcceptIdent();
    AcceptKeyword(KeyWords::assignSy);
    Expression();
    return true;

}

bool CSyntax::Expression()
{
    SimpleExpression();
    if (CheckKeyword(KeyWords::equalSy) || CheckKeyword(KeyWords::lessEqualSy) || CheckKeyword(KeyWords::lessSy) ||
        CheckKeyword(KeyWords::moreEqualSy) || CheckKeyword(KeyWords::moreSy) || CheckKeyword(KeyWords::inSy))
    {
        AcceptKeyword(); // Признает любой символ
        SimpleExpression();
    }
    return true;
}

bool CSyntax::SimpleExpression()
{
    //
    if (CheckKeyword(KeyWords::plusSy))
        AcceptKeyword(KeyWords::plusSy);
    else if (CheckKeyword(KeyWords::minusSy))
        AcceptKeyword(KeyWords::minusSy);
    Term();
    while (CheckKeyword(KeyWords::orSy) || CheckKeyword(KeyWords::plusSy) || CheckKeyword(KeyWords::minusSy))
    {
        AcceptKeyword(); //Признает любой символ
        Term();
    }
    return true;
}

bool CSyntax::Term()
{
    Multiplier();
    while (CheckKeyword(KeyWords::multiplySy) || CheckKeyword(KeyWords::divisionSy) || CheckKeyword(KeyWords::modSy) ||
        CheckKeyword(KeyWords::divSy) || CheckKeyword(KeyWords::andSy))
    {
        AcceptKeyword();
        Multiplier();
    }
    return true;
}

bool CSyntax::Multiplier()
{
    if (CheckKeyword(KeyWords::notSy))
    {
        AcceptKeyword(KeyWords::notSy);
        Multiplier();
    }
    else if (CheckKeyword(KeyWords::bracketOpenSy))
    {
        AcceptKeyword(KeyWords::bracketOpenSy);
        Expression();
        AcceptKeyword(KeyWords::bracketCloseSy);
    }
    else if (CheckIdent())
    {
        AcceptIdent();
    }
    else if (CheckConst())
    {
        AcceptConst();
    }
    return true;
}

bool CSyntax::ProcedureOperator(bool isChecking)
{
    return true;
}


bool CSyntax::ComplexOperator(bool isChecking)
{
    if (ComposeOperator(true))
        ComposeOperator(false);
    else if (ChoosingOperator(true))
        ChoosingOperator(false);
    else if (CycleOperator(true))
        CycleOperator(false);
    else
    {
        return false;
    }


    return true;
}

bool CSyntax::ChoosingOperator(bool isChecking)
{
    if (IfOperator(true))
        IfOperator(false);
    else if ()
}

bool CSyntax::CycleOperator(bool isChecking)
{

}