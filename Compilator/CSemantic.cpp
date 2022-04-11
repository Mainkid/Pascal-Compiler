#include "CSemantic.h"

void CType::AddConstant(CIdent* curConst)
{
	constants.insert(curConst);
}

 CType::CType(ClassType classTypeParam)
{
	classType = classTypeParam;
}

 CIdent::CIdent(std::string nameParam, UsageType usageTypeParam, CType* cTypeParam)
 {
	 name = nameParam;
	 usageType = usageTypeParam;
	 cType = cTypeParam;
 }

 Scope::Scope(Scope* par)
 {
	 parent = par;
	 nullType = parent->nullType;
	 //ScopeInit();
 }

 Scope::Scope()
 {
	 ScopeInit();
 }

 void Scope::AddParameter(CType* param, std::string str)
 {
	 procParam[str].push_back(param);
 }

 CType* Scope:: AddType(ClassType classType)
 {
	 CType* new_type = new CType(classType);
	 typeSet.insert(new_type);
	 return new_type;
 }

 CIdent* Scope::AddIdent(std::string typeStr, UsageType usageType, CType* cType)
 {
	 typeTable[typeStr] = cType;
	 CIdent* new_ident = new CIdent(typeStr, usageType, cType);
	 identTable[typeStr] = new_ident;
	 return new_ident;
 }

 CType* Scope::FindCType(std::string typeStr)
 {
	 Scope* currentScope = this;
	 while (currentScope != NULL)
	 {
		 if (currentScope->typeTable.count(typeStr))
			 return currentScope->typeTable[typeStr];
		 else
			 currentScope = currentScope->parent;
	 }
	 return nullType;
 }

 CIdent* Scope::FindCIdent(std::string identStr)
 {
	 Scope* currentScope = this;
	 while (currentScope != NULL)
	 {
		 if (currentScope->identTable.count(identStr))
			 return currentScope->identTable[identStr];
		 else
			 currentScope = currentScope->parent;
	 }
	 return NULL;
 }

 SemanticError Scope::AddIdent(CIdentToken* ident, UsageType usageType, CIdentToken* typeStr)
 {

	 if (usageType == UsageType::PROC)
	 {
		 return AddProc(ident);

	 }
	 CType* cType = FindCType(typeStr->GetValue());
	 if (!cType)
		 return SemanticError::UnknownType;
	 if (identTable.count(ident->GetValue()) > 0)
		 return SemanticError::AlreadyDeclared;
	 AddIdent(ident->GetValue(), usageType, cType);
	 return SemanticError::NoError;
 }

 SemanticError Scope::AddProc(CIdentToken* ident)
 {
	 if (procTable.count(ident->GetValue()) > 0)
		 return SemanticError::AlreadyDeclared;
	 else if (identTable.count(ident->GetValue()) > 0)
		 return SemanticError::AlreadyDeclared;

	 AddIdent(ident->GetValue(), UsageType::PROC, NULL);
	 procTable[ident->GetValue()] = new Scope(this);
	 //procTable[ident->GetValue()]->parent = this;
	 return SemanticError::NoError;
 }

 SemanticError Scope::CheckProcedureParameters(int pos, CType* ctype, std::string procName)
 {
	 std::list<CType*>::iterator it;
	 int q = procParam[procName].size();
	 if (procParam[procName].size() > pos)
	 {
		 it = procParam[procName].begin();
		 std::advance(it, pos);
	 }
	 else
	 {
		 return SemanticError::WrongParametersValue;
	 }

	 if (*it != ctype)
		 return SemanticError::IncompatibleTypes;

	 return SemanticError::NoError;
 }

 SemanticError Scope::CheckProcParametersAmount(int pos, std::string procName)
 {
	 if (procParam[procName].size() != pos + 1)
		 return SemanticError::WrongParametersValue;
	 else
		 return SemanticError::NoError;
 }

 void Scope::AddConst(CType* type, CIdent* ident)
 {
	 type->AddConstant(ident);
 }

 void Scope::ScopeInit()
 {
	 parent = NULL;
	 nullType = AddType(ClassType::NULLTYPE);

	 AddIdent("integer", UsageType::TYPE, AddType(ClassType::SCALAR));
	 AddIdent("real", UsageType::TYPE, AddType(ClassType::SCALAR));
	 AddIdent("string", UsageType::TYPE, AddType(ClassType::SCALAR)); //?????????????????????
	 AddIdent("boolean", UsageType::TYPE, AddType(ClassType::ENUM));



	 AddConst(typeTable["boolean"], AddIdent("true", UsageType::CONST, typeTable["boolean"]));
	 AddConst(typeTable["boolean"], AddIdent("false", UsageType::CONST, typeTable["boolean"]));
 }