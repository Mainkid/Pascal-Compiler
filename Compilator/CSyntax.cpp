#include "CSyntax.h"





CSyntax::CSyntax()
{
    

    isSkipping = false;
}

CSyntax::~CSyntax()
{

}

void CSyntax::AddNewVariables(CIdentToken* type, UsageType usageType)
{



    while (!identQueue.empty())
    {
        if (!isSkipping && type != NULL)
        {

            if (!ThrowSemanticError(currentScope.get()->AddIdent(dynamic_cast<CIdentToken*>(identQueue.front().get()), usageType, type)))
            {
                generator->pushToVariableQueue(identQueue.front());
            }
        }
        identQueue.pop();
    }
    generator->setVariablesType(type);
}

void CSyntax::AddNewVariablesToBrackets(CIdentToken* type, std::string procName)
{
    while (!identQueue.empty())
    {
        if (!isSkipping && type != NULL)
        {
            CType* ctp = currentScope.get()->FindCType(type->GetValue());
            currentScope.get()->AddParameter(ctp, procName);
            ThrowSemanticError(currentScope.get()->AddIdent(dynamic_cast<CIdentToken*>(identQueue.front().get()), UsageType::VAR, type));
        }
        identQueue.pop();
    }
}

void CSyntax::AddNewProcIdent(CIdentToken* proc)
{
    ThrowSemanticError(currentScope.get()->AddIdent(proc, UsageType::PROC, NULL));
}

bool CSyntax::ThrowSemanticError(SemanticError err)
{

    if (isSkipping)
        return false;

    switch (err)
    {
    case SemanticError::NoError:
        return false;
        break;
    case SemanticError::UnknownType:
        std::cout << "Semantic Error: Unknown type.";
        break;
    case SemanticError::AlreadyDeclared:
        std::cout << "Semantic Error: Var/Type is already declared.";
        break;
    case SemanticError::IncompatibleTypes:
        std::cout << "Semantic Error: Incompatable type.";
        break;
    case SemanticError::WrongParametersValue:
        std::cout << "Semantic Error: Wrong amount of the procedure's parameters";
        break;

    }


    if (semErrLine != -1)
        std::cout << " Line: " << semErrLine << ", Pos: " << semErrPos << std::endl;
    else
        std::cout << " Line: " << lexer.get()->GetLinePos() << ", Pos: " << lexer.get()->GetSymbolPos() << std::endl;
    semErrLine = -1;
    semErrPos = -1;
    return true;
}

CType* CSyntax::GetCType(CIdentToken* tok)
{
    if (!isSkipping && currentScope.get()->FindCType(tok->GetValue()) == currentScope.get()->nullType)
        ThrowSemanticError(SemanticError::UnknownType);

    return currentScope.get()->FindCType(tok->GetValue());
}

CType* CSyntax::CheckSemantic(CType* t1, CType* t2)
{


    if (t1 == t2)
        return t1;
    else if (t1 == currentScope.get()->FindCType("real") && t2 == currentScope.get()->FindCType("integer"))
        return t1;
    else if (t1 == currentScope.get()->FindCType("integer") && t2 == currentScope.get()->FindCType("real"))
        return t2;
    else if (t1 == currentScope.get()->FindCType("boolean")&&(t2 == currentScope.get()->FindCType("false") || t2 == currentScope.get()->FindCType("true")))
        return t1;
    else if (t2 == currentScope.get()->FindCType("boolean") && (t1 == currentScope.get()->FindCType("false") || t1 == currentScope.get()->FindCType("true")))
        return t2;
    else if (t1 == NULL)
        return t2;
    else if (t2 == NULL)
        return t1;
    else
    {
        if (semErrLine == -1)
        {
            semErrLine = lexer.get()->GetLinePos();
            semErrPos = lexer.get()->GetSymbolPos();
        }
        return currentScope.get()->nullType;

    }

}

CType* CSyntax::CheckAssignSemantic(CType* t1, CType* t2)
{
    if (t1 == t2)
        return t1;
    else if (t1 == currentScope.get()->FindCType("real") && t2 == currentScope.get()->FindCType("integer"))
        return t1;
    else if (t1 == currentScope.get()->FindCType("integer") && t2 == currentScope.get()->FindCType("real"))
        return currentScope.get()->nullType;
    else if (t1 == NULL)
        return t2;
    else if (t2 == NULL)
        return t1;
    else
    {
        if (semErrLine == -1)
        {
            semErrLine = lexer.get()->GetLinePos();
            semErrPos = lexer.get()->GetSymbolPos();
        }
        return currentScope.get()->nullType;
    }
}



//------

void CSyntax::GetNextNotEmptyToken()
{

    currentToken = move(lexer.get()->GetNextToken());

    while (currentToken && currentToken.get()->getType() == TokenType::ttKeyword && dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == KeyWords::emptyValueSy)
    {
        currentToken = move(lexer.get()->GetNextToken());
    }
}

std::string CSyntax::GetConstType(std::string constStr)
{
    if (lexer.get()->IsBoolean(constStr))
        return "boolean";
    else if (lexer.get()->IsInteger(constStr))
        return "integer";
    else if (lexer.get()->IsReal(constStr))
        return "real";
    else if (lexer.get()->IsString(constStr))
        return "string";
    else
        return "err";
}

void CSyntax::skipToNextKeyword(KeyWords keyWord, std::set<KeyWords> l)
{
    std::set <KeyWords> ::iterator it;
    if (keyWord == KeyWords::errIdent || keyWord == KeyWords::errConst)
    {
        PrintError(keyWord == KeyWords::errConst, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
    }
    else
        PrintError(keyWord, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
    while (currentToken && !isSkipping)
    {
        GetNextNotEmptyToken();
        if (!currentToken)
            exit(0);
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
            PrintError(keyWord, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
    }
    


    if (!currentToken)
    {
        if (isSkipping)
            PrintError(keyWord, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
        exit(0);
    }
}

void CSyntax::StartSyntaxAnalyze(std::string program)
{
    lexer = std::make_unique<CLexer>(program + ' ');
    //lexer.get() = new Clexer.get()(program + ' ');
    currentScope = std::make_shared<Scope>();
    currentScope = std::make_shared<Scope>(currentScope);
    //Генератор
    generator = std::make_unique <CGenerator>(false);

    GetNextNotEmptyToken();
    Program();
    
    std::cout << "Completed!";
}

CKeywordToken* CSyntax::AcceptKeyword()
{

    CKeywordToken* ct = nullptr;
    isSkipping = false;
    ct = dynamic_cast<CKeywordToken*>(currentToken.get());
    
    int code = static_cast<int>(dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue());
    //std::cout << "ACCEPTED:" << code << std::endl;
    generator->pushToken(currentToken);
    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
    
    return ct;
}

CKeywordToken* CSyntax::AcceptKeyword(KeyWords keyWord, followers cfollowers, std::set<KeyWords> new_followers)
{
    //�������� �� ��, ��� ����� �������� �������� ������
    //CKeywordToken* keyWordToken = dynamic_cast<CKeywordToken*>(currentToken.get());

    CKeywordToken* ct = nullptr;

    cfollowers.l.insert(new_followers.begin(), new_followers.end());

    if (isSkipping && keyWord != skipToKeyWord)
    {
        return ct;
    }
    else if (isSkipping && keyWord == skipToKeyWord)
    {
        isSkipping = false;
    }

    if (currentToken.get()->getType() != TokenType::ttKeyword)
    {
        generator->changeGeneratorStage(GenerationStage::Error);
        skipToNextKeyword(keyWord, cfollowers.l);
    }
    else
    {
        if (dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue() == keyWord)
        {
            ct = dynamic_cast<CKeywordToken*>(currentToken.get());
            int code = static_cast<int>(dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue());
            //std::cout << "ACCEPTED:" << code << std::endl;
        }
        else
        {
            generator->changeGeneratorStage(GenerationStage::Error);
            skipToNextKeyword(keyWord, cfollowers.l);
        }
    }
    generator->pushToken(currentToken);
    
    

    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        if (isSkipping)
            PrintError(keyWord, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
        else
            generator->printCodeToFile();
        exit(0);
    }
    return ct;
}

CTokenPtr CSyntax::AcceptIdent(followers cFollowers, std::set<KeyWords> new_followers)
{
    cFollowers.l.insert(new_followers.begin(), new_followers.end());


    if (isSkipping)
        return NULL;

    CIdentToken* retToken = NULL;

    if (currentToken.get()->getType() != TokenType::ttIdent)
    {
        generator->changeGeneratorStage(GenerationStage::Error);
        skipToNextKeyword(KeyWords::errIdent, cFollowers.l);
    }
    else
    {

        //std::cout << "ACCEPTED: ident(" << dynamic_cast<CIdentToken*>(currentToken.get())->GetValue() << ")" << std::endl;
        
    }
    lastToken = currentToken;
    generator->pushToken(currentToken);
    if (!isSkipping)
        GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
    return lastToken;

}

CIdentToken* CSyntax::AcceptTypeKeywords()
{


    if (isSkipping)
    {
        KeyWords tmp_val = skipToKeyWord;
        if (tmp_val == KeyWords::booleanSy || tmp_val == KeyWords::stringSy || tmp_val == KeyWords::integerSy || tmp_val == KeyWords::realSy)
            isSkipping = false;
    }

    CIdentToken* tmp;

    if (currentScope.get()->FindCIdent(dynamic_cast<CIdentToken*>(currentToken.get())->GetValue())==NULL)
    {
        generator->changeGeneratorStage(GenerationStage::Error);
        std::cout << "��������� �������� ����� �� ���������... " << std::endl;
        return NULL;
    }
    else
    {
        //KeyWords currentKeyword = dynamic_cast<CKeywordToken*>(currentToken.get())->GetValue();
        //if (currentKeyword == KeyWords::booleanSy || currentKeyword == KeyWords::stringSy || currentKeyword == KeyWords::integerSy || currentKeyword == KeyWords::realSy)
            std::cout << "AIGHT" << std::endl;
            tmp = dynamic_cast<CIdentToken*>(currentToken.get());
    }
    lastToken = currentToken;
    GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
    
    return tmp;
}

CType* CSyntax::AcceptConst(followers cFollowers, std::set<KeyWords> KeyWords)
{
    cFollowers.l.insert(KeyWords.begin(), KeyWords.end());
    if (isSkipping)
        return currentScope.get()->nullType;

    CType* retCType = currentScope.get()->nullType;

    if (currentToken.get()->getType() != TokenType::ttConst)
    {
        generator->changeGeneratorStage(GenerationStage::Error);
        skipToNextKeyword(KeyWords::errConst, cFollowers.l);
    }
    else
    {
       // std::cout << "ACCEPTED: const(" << dynamic_cast<CConstToken*>(currentToken.get())->ToString() << ")" << std::endl;
        retCType = currentScope.get()->FindCType(GetConstType(dynamic_cast<CConstToken*>(currentToken.get())->ToString()));
    }
    generator->pushToken(currentToken);
    GetNextNotEmptyToken();
    if (!currentToken)
    {
        exit(0);
    }
    return retCType;
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
        std::cout << "ERROR: Identifier expected" << ". Line: " << line << ", Pos: " << pos << std::endl;
    else
        std::cout << "ERROR: Const expected" << ". Line: " << line << ", Pos: " << pos << std::endl;
}

void CSyntax::PrintError(KeyWords keyword, int line, int pos)
{

    std::cout << "ERROR: Keyword expected " << static_cast<int>(keyword) << ". Line: " << line << ", Pos: " << pos << std::endl;

}

void CSyntax::PrintError(Constructions construction, int line, int pos)
{

    std::cout << "ERROR: Construction expected " << static_cast<int>(construction) << ". Line: " << line << ", Pos: " << pos << std::endl;

}

void CSyntax::Program()
{
    followers cFollowers;
    cFollowers.l.insert(KeyWords::dotSy);

    AcceptKeyword(KeyWords::programSy, cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy });
    AcceptIdent(cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy });
    AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::typeSy,KeyWords::procedureSy });
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
        AcceptKeyword(KeyWords::typeSy, cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::procedureSy });
        TypeDefinition(cFollowers);
        AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::varSy,KeyWords::semicolonSy,KeyWords::procedureSy });
        while (CheckIdent())
        {
            TypeDefinition(cFollowers);
            AcceptKeyword(KeyWords::semicolonSy, cFollowers, {});
        }
    }

}
//TODO: ������ ����
void CSyntax::TypeDefinition(followers cFollowers)
{

    identQueue.push(AcceptIdent(cFollowers, { KeyWords::semicolonSy,KeyWords::procedureSy,KeyWords::beginSy }));
    AcceptKeyword(KeyWords::equalSy, cFollowers, { KeyWords::semicolonSy,KeyWords::procedureSy,KeyWords::beginSy });
    CTokenPtr typeToken = AcceptIdent(cFollowers, {});
    AddNewVariables(dynamic_cast<CIdentToken*>(typeToken.get()), UsageType::TYPE);
    //Type(); //��������

}

void CSyntax::VariableSection(followers cFollowers)
{


    if (CheckKeyword(KeyWords::varSy))
    {
        AcceptKeyword(KeyWords::varSy, cFollowers, { KeyWords::procedureSy,KeyWords::beginSy,KeyWords::semicolonSy });
        OneTypeVariableDefinition(cFollowers);
        AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::semicolonSy,KeyWords::beginSy,KeyWords::procedureSy });
        while (CheckIdent())
        {

            OneTypeVariableDefinition(cFollowers);
            AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::beginSy,KeyWords::procedureSy,KeyWords::semicolonSy });
        }
    }
}

void CSyntax::OneTypeVariableDefinition(followers cFollowers)
{
    //��������� ����� ����������
    identQueue.push(AcceptIdent(cFollowers, { KeyWords::commaSy,KeyWords::semicolonSy,KeyWords::integerSy,KeyWords::realSy,
        KeyWords::booleanSy,KeyWords::stringSy }));


    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        identQueue.push(AcceptIdent(cFollowers, { KeyWords::integerSy,KeyWords::realSy,KeyWords::stringSy,KeyWords::booleanSy,KeyWords::semicolonSy }));
    }

    AcceptKeyword(KeyWords::colonSy, cFollowers, { KeyWords::integerSy,KeyWords::realSy,KeyWords::stringSy,KeyWords::booleanSy,KeyWords::semicolonSy });
    CTokenPtr typeToken = AcceptIdent(cFollowers, {});
    AddNewVariables(dynamic_cast<CIdentToken*>(typeToken.get()), UsageType::VAR);

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
    AcceptKeyword(KeyWords::endSy, cFollowers, {});
}


void CSyntax::Operator(followers cFollowers)
{
    UnmarkedOperator(cFollowers);
}

void CSyntax::UnmarkedOperator(followers cFollowers)
{
    if (CheckIdent()||CheckKeyword(KeyWords::writeLN))
        SimpleOperator(cFollowers);
    else if (CheckKeyword(KeyWords::beginSy) || CheckKeyword(KeyWords::ifSy)
        || CheckKeyword(KeyWords::caseSy) || CheckKeyword(KeyWords::whileSy))
        ComplexOperator(cFollowers);
    else
        return;
}
//TODO: ���������� ����������� ���������!
void CSyntax::SimpleOperator(followers cFollowers)
{
    CType* t1 = NULL;
    CType* t2 = NULL;
    std::string procName = "";
    if (!CheckKeyword(KeyWords::writeLN))
    {
        generator->changeGeneratorStage(GenerationStage::AssignableVar);
        CTokenPtr tokenTMP = AcceptIdent(cFollowers, { KeyWords::assignSy,KeyWords::semicolonSy });
        t1 = GetCType(dynamic_cast<CIdentToken*>(tokenTMP.get()));
        procName = dynamic_cast<CIdentToken*>(tokenTMP.get())->GetValue();
        if (CheckKeyword(KeyWords::assignSy))
        {
            //AcceptKeyword(KeyWords::assignSy, cFollowers, {});
            //Генератор, режим работы
            generator->changeGeneratorStage(GenerationStage::Nothing);
            t2 = AssignOperator(cFollowers);
        }
        else if (CheckKeyword(KeyWords::bracketOpenSy))
        {
            ProcedureOperator(cFollowers, procName);
            return;
        }
        else if (t1 == NULL)
        {


            ThrowSemanticError(currentScope.get()->CheckProcParametersAmount(-1, procName));
            return;
        }
        else
            return;

        if (CheckAssignSemantic(t1, t2) == currentScope.get()->nullType)
            ThrowSemanticError(SemanticError::IncompatibleTypes);
    }
    else
    {
        
        AcceptKeyword();
        AcceptKeyword(KeyWords::bracketOpenSy, {}, {});
        generator->changeGeneratorStage(GenerationStage::WRITELN);
        if (CheckIdent())
            AcceptIdent({}, {});
        else
            AcceptConst({}, {});
        AcceptKeyword(KeyWords::bracketCloseSy, {}, {});
    }



}

//�������� �� ���������!!!
CType* CSyntax::AssignOperator(followers cFollowers)
{
    
    AcceptKeyword(KeyWords::assignSy, cFollowers, { KeyWords::semicolonSy });
    generator->changeGeneratorStage(GenerationStage::Expression);
    CType* tmp= Expression(cFollowers);
    generator->changeGeneratorStage(GenerationStage::ExpressionEND);
    return tmp;

}

CType* CSyntax::Expression(followers cFollowers)
{
    CType* t1 = NULL;
    CType* t2 = NULL;
    
    t1 = SimpleExpression(cFollowers);
    if (CheckKeyword(KeyWords::equalSy) || CheckKeyword(KeyWords::lessEqualSy) || CheckKeyword(KeyWords::lessSy) ||
        CheckKeyword(KeyWords::moreEqualSy) || CheckKeyword(KeyWords::moreSy) || CheckKeyword(KeyWords::inSy) || CheckKeyword(KeyWords::notEqualSy))
    {
        AcceptKeyword();
        t2 = SimpleExpression(cFollowers);
        if (CheckSemantic(t1, t2) != currentScope.get()->nullType)
            return currentScope.get()->FindCType("boolean");
        else return currentScope.get()->nullType;
    }
    
    return t1;

}

CType* CSyntax::SimpleExpression(followers cFollowers)
{

    CType* t1 = NULL;
    CType* t2 = NULL;

    t1 = Term(cFollowers);
    while (CheckKeyword(KeyWords::orSy) || CheckKeyword(KeyWords::plusSy) || CheckKeyword(KeyWords::minusSy))
    {
        AcceptKeyword();
        t2 = Term(cFollowers);
        if (CheckKeyword(KeyWords::orSy))
        {
            if (t1 != currentScope.get()->FindCType("boolean") || t2 != currentScope.get()->FindCType("boolean"))
                t1 = currentScope.get()->nullType;
        }
        else if (CheckKeyword(KeyWords::plusSy)||CheckKeyword(KeyWords::minusSy))
        {
            if (t1 == currentScope.get()->FindCType("boolean") || t2 == currentScope.get()->FindCType("boolean") ||
                t1 == currentScope.get()->FindCType("string")||t2==currentScope.get()->FindCType("string"))
                t1 = currentScope.get()->nullType;
        }

        t1 = CheckSemantic(t1, t2);
    }
    return t1;
}

CType* CSyntax::Term(followers cFollowers)
{
    CType* t1 = NULL;
    CType* t2 = NULL;

    t1 = Multiplier(cFollowers);
    while (CheckKeyword(KeyWords::multiplySy) || CheckKeyword(KeyWords::divisionSy) || CheckKeyword(KeyWords::modSy) ||
        CheckKeyword(KeyWords::divSy) || CheckKeyword(KeyWords::andSy))
    {
        AcceptKeyword();
        t2 = Multiplier(cFollowers);

        if (CheckKeyword(KeyWords::multiplySy) || CheckKeyword(KeyWords::divisionSy))
        {
            if (t1 != currentScope.get()->FindCType("integer") && t1 != currentScope.get()->FindCType("real"))
                t1 = currentScope.get()->nullType;
        }
        else if (CheckKeyword(KeyWords::divSy) || CheckKeyword(KeyWords::modSy))
        {
            if (t1 != currentScope.get()->FindCType("integer") || t2 != currentScope.get()->FindCType("integer"))
                t1 = currentScope.get()->nullType;
            
        }
        else if (CheckKeyword(KeyWords::andSy) || CheckKeyword(KeyWords::orSy))
        {
            if (t1 != currentScope.get()->FindCType("boolean"))
                t1 = currentScope.get()->nullType;
        }


        t1 = CheckSemantic(t1, t2);
    }
    return t1;
}

CType* CSyntax::Multiplier(followers cFollowers)
{
    cFollowers.l.insert(KeyWords::minusSy);
    cFollowers.l.insert(KeyWords::multiplySy);
    cFollowers.l.insert(KeyWords::divisionSy);
    cFollowers.l.insert(KeyWords::modSy);
    cFollowers.l.insert(KeyWords::divSy);
    cFollowers.l.insert(KeyWords::andSy);
    CType* ctype = NULL;


    if (CheckKeyword(KeyWords::notSy))
    {
        AcceptKeyword();
        return Multiplier(cFollowers);
    }
    else if (CheckKeyword(KeyWords::bracketOpenSy))
    {
        AcceptKeyword();
        ctype = Expression(cFollowers);
        AcceptKeyword(KeyWords::bracketCloseSy, cFollowers, { KeyWords::semicolonSy });
        return ctype;
    }
    else if (CheckIdent())
    {
        CTokenPtr tmp = AcceptIdent(cFollowers, {});
        ctype= GetCType(dynamic_cast<CIdentToken*>(tmp.get()));
        //generator->pushToken(tmp);
        return ctype;
    }
    else if (CheckConst())
    {

        return AcceptConst(cFollowers, {});
    }
    else
    {
        PrintError(false, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
        return currentScope.get()->nullType;
    }
}

void CSyntax::ProcedureOperator(followers cFollowers, std::string procedureName)
{
    AcceptKeyword(KeyWords::bracketOpenSy, cFollowers, { KeyWords::bracketCloseSy,KeyWords::semicolonSy });
    FactParameter(cFollowers,procedureName);
    AcceptKeyword(KeyWords::bracketCloseSy, cFollowers, { KeyWords::semicolonSy });
}

void CSyntax::FactParameter(followers cFollowers, std::string procedureName)
{

    CType* t1=NULL;

    int pos = 0;

    if (CheckIdent())
        t1=currentScope.get()->FindCType(dynamic_cast<CIdentToken*>( AcceptIdent(cFollowers, {}).get())->GetValue());
    else
        t1=Expression(cFollowers);

    
    

    while (CheckKeyword(KeyWords::commaSy))
    {
        ThrowSemanticError(currentScope.get()->CheckProcedureParameters(pos, t1, procedureName));
        AcceptKeyword();
        if (CheckIdent())
            t1= currentScope.get()->FindCType(dynamic_cast<CIdentToken*>(AcceptIdent(cFollowers, {}).get())->GetValue());
        else
            t1=Expression(cFollowers);
        pos++;
    }
    ThrowSemanticError(currentScope.get()->CheckProcedureParameters(pos, t1, procedureName));

    ThrowSemanticError(currentScope.get()->CheckProcParametersAmount(pos, procedureName));
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
    {
        //PrintError(Constructions::no, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
        return;
    }



}

void CSyntax::ChoosingOperator(followers cFollowers)
{


    if (CheckKeyword(KeyWords::ifSy))
        IfOperator(cFollowers);
    else if (CheckKeyword(KeyWords::caseSy))
        ChoiseOperator(cFollowers);
    else
    {
        PrintError(Constructions::no, lexer.get()->GetLinePos(), lexer.get()->GetSymbolPos());
        return;
    }
}

void CSyntax::CycleOperator(followers cFollowers)
{
    generator->changeGeneratorStage(GenerationStage::WhileSTART);
    AcceptKeyword(KeyWords::whileSy, cFollowers, { KeyWords::doSy,KeyWords::semicolonSy,KeyWords::bracketOpenSy });
    generator->changeGeneratorStage(GenerationStage::Expression);
    CType* tmp = Expression(cFollowers);
    generator->changeGeneratorStage(GenerationStage::ExpressionLogicalEND);
    if (tmp != currentScope.get()->FindCType("boolean"))
        ThrowSemanticError(SemanticError::IncompatibleTypes);
    AcceptKeyword(KeyWords::doSy, cFollowers, { KeyWords::semicolonSy });
    UnmarkedOperator(cFollowers);
    generator->changeGeneratorStage(GenerationStage::WhileEND);
}

void CSyntax::IfOperator(followers cFollowers)
{
    generator->changeGeneratorStage(GenerationStage::IFSTART);
    AcceptKeyword(KeyWords::ifSy, cFollowers, { KeyWords::thenSy,KeyWords::elseSy,KeyWords::semicolonSy });
    generator->changeGeneratorStage(GenerationStage::THENSTART);
    generator->changeGeneratorStage(GenerationStage::Expression);
    CType* tmp = Expression(cFollowers);
    generator->changeGeneratorStage(GenerationStage::ExpressionLogicalIFEND);
    if (tmp != currentScope.get()->FindCType("boolean"))
        ThrowSemanticError(SemanticError::IncompatibleTypes);
    AcceptKeyword(KeyWords::thenSy, cFollowers, { KeyWords::elseSy,KeyWords::semicolonSy });
    generator->changeGeneratorStage(GenerationStage::Expression);
    UnmarkedOperator(cFollowers);
    generator->changeGeneratorStage(GenerationStage::ExpressionIfEnd);
    if (CheckKeyword(KeyWords::elseSy))
    {
        generator->changeGeneratorStage(GenerationStage::ELSESTART);
        AcceptKeyword();
        generator->changeGeneratorStage(GenerationStage::Expression);
        UnmarkedOperator(cFollowers);
        generator->changeGeneratorStage(GenerationStage::ExpressionIfEnd);
    }
    generator->changeGeneratorStage(GenerationStage::IfEND);
}

void CSyntax::ChoiseOperator(followers cFollowers)
{
    CType* t1 = NULL;
    CType* t2 = NULL;
    AcceptKeyword(KeyWords::caseSy, cFollowers, { KeyWords::ofSy,KeyWords::semicolonSy });
    t1 = Expression(cFollowers);
    AcceptKeyword(KeyWords::ofSy, cFollowers, { KeyWords::semicolonSy });
    t2 = VariantListElement(cFollowers);
    while (CheckKeyword(KeyWords::semicolonSy))
    {
        //if (t2==currentScope.get()->nullType)
          //  ThrowSemanticError(SemanticError::)
        if (CheckAssignSemantic(t1, t2) == currentScope.get()->nullType)
            ThrowSemanticError(SemanticError::IncompatibleTypes);

        AcceptKeyword();
        t2 = VariantListElement(cFollowers);

    }
    if (CheckAssignSemantic(t1, t2) == currentScope.get()->nullType)
        ThrowSemanticError(SemanticError::IncompatibleTypes);
    AcceptKeyword(KeyWords::endSy, cFollowers, { KeyWords::colonSy,KeyWords::endSy });

}

CType* CSyntax::VariantListElement(followers cFollowers)
{
    CType* ret = NULL;
    if (CheckConst())
        ret = ListVariantLabels(cFollowers);
    else
        return NULL;
    AcceptKeyword(KeyWords::colonSy, cFollowers, {});
    Operator(cFollowers);
    return ret;
}

CType* CSyntax::ListVariantLabels(followers cFollowers)
{
    CType* t1 = NULL;
    CType* t2 = NULL;
    t1 = AcceptConst(cFollowers, { KeyWords::commaSy });
    while (CheckKeyword(KeyWords::commaSy))
    {
        t1 = CheckSemantic(t1, t2);
        AcceptKeyword();
        t2 = AcceptConst(cFollowers, { KeyWords::commaSy });

    }
    return CheckSemantic(t1, t2);

}


void CSyntax::ProcedureDescription(followers cFollowers)
{

}

void CSyntax::ProcedureHeader(followers cFollowers)
{

    AcceptKeyword(KeyWords::procedureSy, cFollowers, { KeyWords::beginSy });
    CTokenPtr q = AcceptIdent(cFollowers, {});
    CIdentToken* cit = dynamic_cast<CIdentToken*>(q.get());
    AddNewProcIdent(cit);
    std::string procName = cit->GetValue();
    currentScope = std::make_shared<Scope>(currentScope);
    
    

    if (CheckKeyword(KeyWords::bracketOpenSy))
    {
        AcceptKeyword();
        FormalOperatorsSection(cFollowers,procName);
        while (CheckKeyword(KeyWords::semicolonSy))
        {
            AcceptKeyword();
            FormalOperatorsSection(cFollowers, procName);
        }
        AcceptKeyword(KeyWords::bracketCloseSy, cFollowers, { KeyWords::beginSy,KeyWords::varSy,KeyWords::semicolonSy });
    }
    AcceptKeyword(KeyWords::semicolonSy, cFollowers, { KeyWords::beginSy });
}

void CSyntax::FormalOperatorsSection(followers cFollowers,std::string procedureName)
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
        ParametersGroup(cFollowers,procedureName);
}

void CSyntax::ParametersGroup(followers cFollowers)
{
    identQueue.push( AcceptIdent(cFollowers, { KeyWords::commaSy }));
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        identQueue.push(AcceptIdent(cFollowers, { KeyWords::commaSy }));
    }
    AcceptKeyword(KeyWords::colonSy, cFollowers, { KeyWords::integerSy,KeyWords::booleanSy,KeyWords::stringSy,KeyWords::realSy });
    CIdentToken* cit = AcceptTypeKeywords();
    //currentScope.get()->AddParameter(currentScope.get()->FindCType(cit->GetValue()));
    AddNewVariables(cit, UsageType::VAR);

}

void CSyntax::ParametersGroup(followers cFollowers,std::string procName)
{
    identQueue.push(AcceptIdent(cFollowers, { KeyWords::commaSy }));
    while (CheckKeyword(KeyWords::commaSy))
    {
        AcceptKeyword();
        identQueue.push(AcceptIdent(cFollowers, { KeyWords::commaSy }));
    }
    AcceptKeyword(KeyWords::colonSy, cFollowers, { KeyWords::integerSy,KeyWords::booleanSy,KeyWords::stringSy,KeyWords::realSy });
    CIdentToken* cit = AcceptTypeKeywords();
    AddNewVariablesToBrackets(cit, procName);
    AddNewVariables(cit, UsageType::VAR);

}

