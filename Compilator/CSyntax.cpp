#include "CSyntax.h"





CSyntax::CSyntax()
{
    isSkipping = false;
}

CSyntax::~CSyntax()
{

}

void CSyntax::GetNextNotEmptyToken()
{

    currentToken = move(lexer->GetNextToken());
    while (currentToken&& currentToken.get()->getType() == TokenType::ttKeyword && dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == KeyWords::emptyValueSy)
    {
        currentToken = move(lexer->GetNextToken());
    }
}

void CSyntax::skipToNextKeyword(KeyWords keyWord, std::set<KeyWords> l)
{
    std::set <KeyWords> ::iterator it;
    if (keyWord == KeyWords::errIdent||keyWord==KeyWords::errConst)
    {
        PrintError(keyWord == KeyWords::errConst, lexer->GetLinePos(), lexer->GetSymbolPos());
    }
    else
        PrintError(keyWord, lexer->GetLinePos(), lexer->GetSymbolPos());
    while (currentToken && !isSkipping)
    {
        GetNextNotEmptyToken();
        for (it = l.begin(); it != l.end(); ++it)
        {
            if (currentToken.get()->getType() == TokenType::ttKeyword && *it == dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue())
            {
                
                isSkipping = true;
                skipToKeyWord = *it;
                return;
            }
        }

        if (!currentToken)
            PrintError(keyWord, lexer->GetLinePos(), lexer->GetSymbolPos());
    }



    if (!currentToken)
    {
        if (isSkipping)
            PrintError(keyWord, lexer->GetLinePos(), lexer->GetSymbolPos());
        exit(0);
    }
}

void CSyntax::StartSyntaxAnalyze(std::string program)
{
    lexer = new CLexer(program + ' ');
    GetNextNotEmptyToken();
    Program();

}

void CSyntax::AcceptKeyword()
{

    isSkipping = false;
    int code = static_cast<int>(dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue());
    std::cout << "ACCEPTED:"<<code << std::endl;
    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
}

void CSyntax::AcceptKeyword(KeyWords keyWord, followers cfollowers, std::set<KeyWords> new_followers)
{
    //Проверка на то, что токен является ключевым словом
    //CKeywordToken* keyWordToken = dynamic_cast<CKeywordToken*>(currentToken.get());
    cfollowers.l.insert(new_followers.begin(), new_followers.end());

    if (isSkipping&&keyWord!=skipToKeyWord)
    {
        return;
    }
    else if (isSkipping && keyWord == skipToKeyWord)
    {
        isSkipping = false;
    }
    
    if (currentToken.get()->getType()!=TokenType::ttKeyword)
    {
        skipToNextKeyword(keyWord, cfollowers.l);
    }
    else
    {
        if (dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == keyWord)
        {
            int code = static_cast<int>(dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue());
            std::cout << "ACCEPTED:" <<code<< std::endl;
        }
        else
        {
           
            skipToNextKeyword(keyWord, cfollowers.l);
        }
    }

    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        if (isSkipping)
            PrintError(keyWord, lexer->GetLinePos(), lexer->GetSymbolPos());
        exit(0);
    }

}

void CSyntax::AcceptIdent(followers cFollowers, std::set<KeyWords> new_followers)
{
    cFollowers.l.insert(new_followers.begin(), new_followers.end());

    if (isSkipping)
        return;


    //CIdentToken* identToken = dynamic_cast<CIdentToken*>(currentToken.get());
    if (currentToken.get()->getType() != TokenType::ttIdent)
    {
        //std::cout << "Ожидался идентификатор..." << std::endl;
        skipToNextKeyword(KeyWords::errIdent, cFollowers.l);
    }
    else
    {
        std::cout << "ACCEPTED: ident("<< dynamic_cast<CIdentToken*>(currentToken.get())->GetValue() <<")"<< std::endl;
    }
    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }

}

void CSyntax::AcceptTypeKeywords()
{


    if (isSkipping )
    {
        KeyWords tmp_val = skipToKeyWord;
        if (tmp_val == KeyWords::booleanSy || tmp_val == KeyWords::stringSy || tmp_val == KeyWords::integerSy || tmp_val == KeyWords::realSy)
            isSkipping = false;
    }

    if (currentToken.get()->getType() != TokenType::ttKeyword)
    {
        std::cout << "Ожидалось ключевое слово со значением... " << std::endl;
    }
    else
    {
        KeyWords currentKeyword = dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue();
        if (currentKeyword == KeyWords::booleanSy ||currentKeyword == KeyWords::stringSy || currentKeyword == KeyWords::integerSy || currentKeyword == KeyWords::realSy)
            std::cout << "AIGHT" << std::endl;
    }
    GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
}

void CSyntax::AcceptConst(followers cFollowers, std::set<KeyWords> KeyWords)
{
    cFollowers.l.insert(KeyWords.begin(), KeyWords.end());
    if (isSkipping)
        return;
    if (currentToken.get()->getType() != TokenType::ttConst)
    {
        skipToNextKeyword(KeyWords::errConst, cFollowers.l);
    }
    else
    {
        std::cout << "ACCEPTED: const(" << dynamic_cast<CConstToken*>(currentToken.get())->ToString() <<")"<< std::endl;
    }
    GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
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

void CSyntax::SkipToKeyWord(KeyWords keyWord)
{

    while (currentToken)
    {

        GetNextNotEmptyToken();
        if (currentToken.get()->getType() == TokenType::ttKeyword &&
            dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == keyWord)
            return;
    }
}

void CSyntax::PrintError(bool isConst, int line, int pos)
{
    if (!isConst)
        std::cout << "ERROR: Ожидался идентификатор" << ". Строка: " << line << ", символ: " << pos<< std::endl;
    else
        std::cout << "ERROR: Ожидалась константа" << ". Строка: " << line << ", символ: " << pos << std::endl;
}

void CSyntax::PrintError(KeyWords keyword,int line, int pos)
{

    std::cout << "ERROR: Ожидалось ключевое слово " <<static_cast<int>(keyword) <<". Строка: "<<line<<", символ: "<<pos<< std::endl;

}

void CSyntax::PrintError(Constructions construction, int line, int pos)
{

    std::cout << "ERROR: Ожидалась конструкция " << static_cast<int>(construction) << ". Строка: " << line << ", символ: " << pos << std::endl;

}

void CSyntax::Program()
{
    followers cFollowers;
    cFollowers.l.insert(KeyWords::dotSy);

    AcceptKeyword(KeyWords::programSy, cFollowers, {KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy});
    AcceptIdent(cFollowers,{KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy});
    AcceptKeyword(KeyWords::semicolonSy,cFollowers, {KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy});
    Block(cFollowers);
    AcceptKeyword(KeyWords::dotSy, cFollowers, {});
    
}

void CSyntax::Block(followers cFollowers)
{
    TypeSection(cFollowers);
    VariableSection(cFollowers);
    ProcedureSection(cFollowers);
    //OperatorSection();    
    ComposeOperator(cFollowers);
}
//TODO: followers
void CSyntax::TypeSection(followers cFollowers)
{

    if (CheckKeyword(KeyWords::typeSy))
    {
        AcceptKeyword(KeyWords::typeSy,cFollowers, {KeyWords::varSy,KeyWords::semicolonSy,KeyWords::procedureSy});
        TypeDefinition(cFollowers);
        AcceptKeyword(KeyWords::semicolonSy,cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::procedureSy });
        while (CheckIdent())
        {
            TypeDefinition(cFollowers);
            AcceptKeyword(KeyWords::semicolonSy,cFollowers, {});
        }
    }
    
}
//TODO: Другие типы
void CSyntax::TypeDefinition(followers cFollowers)
{

    AcceptIdent(cFollowers,{KeyWords::semicolonSy,KeyWords::procedureSy,KeyWords::beginSy});
    AcceptKeyword(KeyWords::equalSy,cFollowers, {KeyWords::semicolonSy,KeyWords::procedureSy,KeyWords::beginSy});
    //Type(); //Доделать

}

void CSyntax::VariableSection(followers cFollowers)
{
    if (CheckKeyword(KeyWords::varSy))
    {
        AcceptKeyword(KeyWords::varSy, cFollowers, {KeyWords::procedureSy,KeyWords::beginSy,KeyWords::semicolonSy});
        OneTypeVariableDefinition(cFollowers);
        AcceptKeyword(KeyWords::semicolonSy, cFollowers, {KeyWords::semicolonSy,KeyWords::beginSy,KeyWords::procedureSy});
        while (CheckIdent())
        {
            OneTypeVariableDefinition(cFollowers);
            AcceptKeyword(KeyWords::semicolonSy, cFollowers, {KeyWords::beginSy,KeyWords::procedureSy,KeyWords::semicolonSy});
        }
    }
}

void CSyntax::OneTypeVariableDefinition(followers cFollowers)
{

    AcceptIdent(cFollowers,{KeyWords::commaSy,KeyWords::semicolonSy,KeyWords::integerSy,KeyWords::realSy,
        KeyWords::booleanSy,KeyWords::stringSy});
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        AcceptIdent(cFollowers, { KeyWords::integerSy,KeyWords::realSy,KeyWords::stringSy,KeyWords::booleanSy,KeyWords::semicolonSy });
    }
    AcceptKeyword(KeyWords::colonSy, cFollowers, {KeyWords::integerSy,KeyWords::realSy,KeyWords::stringSy,KeyWords::booleanSy,KeyWords::semicolonSy});
    AcceptTypeKeywords();
}
//DO
void CSyntax::ProcedureSection(followers cFollowers)
{
    while (CheckKeyword(KeyWords::procedureSy))
    {
        ProcedureHeader(cFollowers);
        Block(cFollowers);
        AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::beginSy });
    }

}
//DO
void CSyntax::OperatorSection(followers cFollowers)
{
    ComposeOperator(cFollowers);
}

void CSyntax::ComposeOperator(followers cFollowers)
{
    cFollowers.l.insert(KeyWords::beginSy);
    cFollowers.l.insert(KeyWords::endSy);

    AcceptKeyword(KeyWords::beginSy, cFollowers, {});
    Operator(cFollowers);
    while (CheckKeyword(KeyWords::semicolonSy))
    {
        AcceptKeyword(KeyWords::semicolonSy, cFollowers, {});
        Operator(cFollowers);
    }
    AcceptKeyword(KeyWords::endSy,cFollowers,{});
}


void CSyntax::Operator(followers cFollowers)
{
    UnmarkedOperator(cFollowers);
}

void CSyntax::UnmarkedOperator(followers cFollowers)
{
    if (CheckIdent())
        SimpleOperator(cFollowers);
    else if (CheckKeyword(KeyWords::beginSy) || CheckKeyword(KeyWords::ifSy)
        || CheckKeyword(KeyWords::caseSy) || CheckKeyword(KeyWords::whileSy))
        ComplexOperator(cFollowers);
    else
        return;
}
//TODO: Необходимо рассмотреть процедуру!
void CSyntax::SimpleOperator(followers cFollowers)
{
    
    AcceptIdent(cFollowers, { KeyWords::assignSy,KeyWords::semicolonSy });
    if (CheckKeyword(KeyWords::assignSy))
        AssignOperator(cFollowers);
    else if (CheckKeyword(KeyWords::bracketOpenSy))
        ProcedureOperator(cFollowers);
    else
        return;
    //if (AssignOperator(true))
    //AssignOperator( cFollowers);
    //else
        //ProcedureOperator(false);
    //return true;


    
}

//Поправка на процедуру!!!
void CSyntax::AssignOperator(followers cFollowers)
{

    AcceptKeyword(KeyWords::assignSy,cFollowers,{KeyWords::semicolonSy});
    Expression(cFollowers);

}

void CSyntax::Expression(followers cFollowers)
{
    SimpleExpression(cFollowers);
    if (CheckKeyword(KeyWords::equalSy) || CheckKeyword(KeyWords::lessEqualSy) || CheckKeyword(KeyWords::lessSy) ||
        CheckKeyword(KeyWords::moreEqualSy) || CheckKeyword(KeyWords::moreSy) || CheckKeyword(KeyWords::inSy))
    {
        AcceptKeyword(); 
        SimpleExpression(cFollowers);
    }

}

void CSyntax::SimpleExpression(followers cFollowers)
{
    //
    //if (CheckKeyword(KeyWords::plusSy))
        //AcceptKeyword(KeyWords::plusSy);
    //else if (CheckKeyword(KeyWords::minusSy))
        //AcceptKeyword(KeyWords::minusSy);
    Term(cFollowers);
    while (CheckKeyword(KeyWords::orSy) || CheckKeyword(KeyWords::plusSy) || CheckKeyword(KeyWords::minusSy))
    {
        AcceptKeyword(); 
        Term(cFollowers);
    }
}

void CSyntax::Term(followers cFollowers)
{
    Multiplier(cFollowers);
    while (CheckKeyword(KeyWords::multiplySy) || CheckKeyword(KeyWords::divisionSy) || CheckKeyword(KeyWords::modSy) ||
        CheckKeyword(KeyWords::divSy) || CheckKeyword(KeyWords::andSy))
    {
        AcceptKeyword();
        Multiplier(cFollowers);
    }
}

void CSyntax::Multiplier(followers cFollowers)
{
    cFollowers.l.insert(KeyWords::minusSy);
    cFollowers.l.insert(KeyWords::multiplySy);
    cFollowers.l.insert(KeyWords::divisionSy);
    cFollowers.l.insert(KeyWords::modSy);
    cFollowers.l.insert(KeyWords::divSy);
    cFollowers.l.insert(KeyWords::andSy);


    if (CheckKeyword(KeyWords::notSy))
    {
        AcceptKeyword();
        Multiplier(cFollowers);
    }
    else if (CheckKeyword(KeyWords::bracketOpenSy))
    {
        AcceptKeyword();
        Expression(cFollowers);
        AcceptKeyword(KeyWords::bracketCloseSy,cFollowers,{KeyWords::semicolonSy});
    }
    else if (CheckIdent())
    {
        AcceptIdent(cFollowers, {});
    }
    else if (CheckConst())
    {
        AcceptConst(cFollowers, {});
    }
    else
        return;
}

void CSyntax::ProcedureOperator(followers cFollowers)
{
    AcceptKeyword(KeyWords::bracketOpenSy, cFollowers, { KeyWords::bracketCloseSy,KeyWords::semicolonSy });
    FactParameter(cFollowers);
    AcceptKeyword(KeyWords::bracketCloseSy, cFollowers, { KeyWords::semicolonSy });
}

void CSyntax::FactParameter(followers cFollowers)
{
    if (CheckIdent())
        AcceptIdent(cFollowers, {});
    else
        Expression(cFollowers);

    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        if (CheckIdent())
            AcceptIdent(cFollowers, {});
        else
            Expression(cFollowers);
    }
}

void CSyntax::ComplexOperator(followers cFollower)
{
    if (CheckKeyword(KeyWords::beginSy))
        ComposeOperator(cFollower);
    else if (CheckKeyword(KeyWords::ifSy) || CheckKeyword(KeyWords::caseSy))
        ChoosingOperator(cFollower);
    else if (CheckKeyword(KeyWords::whileSy))
        CycleOperator(cFollower);
    else
        return;



}

void CSyntax::ChoosingOperator(followers cFollowers)
{


    if (CheckKeyword(KeyWords::ifSy))
        IfOperator(cFollowers);
    else if (CheckKeyword(KeyWords::caseSy))
        ChoiseOperator(cFollowers);
    else
        return;
}

void CSyntax::CycleOperator(followers cFollowers)
{
    AcceptKeyword(KeyWords::whileSy, cFollowers, { KeyWords::doSy,KeyWords::semicolonSy,KeyWords::bracketOpenSy });
    Expression(cFollowers);
    AcceptKeyword(KeyWords::doSy,cFollowers,{KeyWords::semicolonSy});
    UnmarkedOperator(cFollowers);
}

void CSyntax::IfOperator(followers cFollowers)
{

    AcceptKeyword(KeyWords::ifSy,cFollowers,{KeyWords::thenSy,KeyWords::elseSy,KeyWords::semicolonSy});
    Expression(cFollowers);
    AcceptKeyword(KeyWords::thenSy,cFollowers,{KeyWords::elseSy,KeyWords::semicolonSy});
    UnmarkedOperator(cFollowers);
    if (CheckKeyword(KeyWords::elseSy))
    {
        AcceptKeyword();
        UnmarkedOperator(cFollowers);
    }
}

void CSyntax::ChoiseOperator(followers cFollowers)
{

    AcceptKeyword(KeyWords::caseSy,cFollowers,{KeyWords::ofSy,KeyWords::semicolonSy});
    Expression(cFollowers);
    AcceptKeyword(KeyWords::ofSy,cFollowers,{KeyWords::semicolonSy});
    VariantListElement(cFollowers);
    while (CheckKeyword(KeyWords::semicolonSy))
    {
        AcceptKeyword();
        VariantListElement(cFollowers);
    }
    AcceptKeyword(KeyWords::endSy,cFollowers,{KeyWords::colonSy,KeyWords::endSy});

}

void CSyntax::VariantListElement(followers cFollowers)
{
    if (CheckConst())
        ListVariantLabels(cFollowers);
    else
        return;
    AcceptKeyword(KeyWords::colonSy, cFollowers, {});
    Operator(cFollowers);
}

void CSyntax::ListVariantLabels(followers cFollowers)
{

    AcceptConst(cFollowers,{KeyWords::commaSy});
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        AcceptConst(cFollowers, { KeyWords::commaSy });
    }
}


void CSyntax::ProcedureDescription(followers cFollowers)
{

}

void CSyntax::ProcedureHeader(followers cFollowers)
{

    AcceptKeyword(KeyWords::procedureSy, cFollowers, { KeyWords::beginSy });
    AcceptIdent(cFollowers, {});
    if (CheckKeyword(KeyWords::bracketOpenSy))
    {
        AcceptKeyword();
        FormalOperatorsSection(cFollowers);
        while (CheckKeyword(KeyWords::semicolonSy))
        {
            AcceptKeyword();
            FormalOperatorsSection(cFollowers);
        }
        AcceptKeyword(KeyWords::bracketCloseSy, cFollowers, { KeyWords::beginSy,KeyWords::varSy,KeyWords::semicolonSy });
    }
    AcceptKeyword(KeyWords::semicolonSy, cFollowers, {KeyWords::beginSy});
}

void CSyntax::FormalOperatorsSection(followers cFollowers)
{
    if (CheckKeyword(KeyWords::varSy))
    {
        AcceptKeyword();
        ParametersGroup(cFollowers);
    }
    else if (CheckKeyword(KeyWords::procedureSy))
    {
        AcceptKeyword();
        AcceptIdent(cFollowers, {});
        while (CheckKeyword(KeyWords::commaSy))
        {
            AcceptKeyword();
            AcceptIdent(cFollowers, { KeyWords::commaSy });
        }
    }
    else
        ParametersGroup(cFollowers);
}

void CSyntax::ParametersGroup(followers cFollowers)
{
    AcceptIdent(cFollowers,{KeyWords::commaSy});
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        AcceptIdent(cFollowers, { KeyWords::commaSy });
    }
    AcceptKeyword(KeyWords::colonSy, cFollowers, { KeyWords::integerSy,KeyWords::booleanSy,KeyWords::stringSy,KeyWords::realSy });
    AcceptTypeKeywords();

}