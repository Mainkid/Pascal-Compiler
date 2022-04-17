






#pragma once
#include <memory>
#include <iostream>
#include <list>
#include <set>
#include <iterator>
#include "CToken.h"
#include <queue>
#include <stack>
#include <windows.h>
#include <WinUser.h>
#include <WinBase.h>



class TreeNode;

typedef std::shared_ptr<TreeNode> TreeNodePtr;

enum class GenerationStage
{
	Error,
	Nothing,
	Var,
	Expression,
	ExpressionEND,
	AssignableVar,
	ExpressionLogicalEND,
	WhileSTART,
	WhileEND
};



class TreeNode
{
public:
	TreeNodePtr childL=nullptr;
	TreeNodePtr childR = nullptr;
	CTokenPtr tok = nullptr;
	std::string constant = "";

	TreeNode()
	{

	}
};



class CGenerator
{
private:
	GenerationStage generationStage=GenerationStage::Nothing;
	
public:
	

	std::queue<CTokenPtr> expressionQueue; //для записи символов из анализатора

	std::queue<CTokenPtr> exprQ; //poland notation
	CTokenPtr assignedIdent; //Идентификатор присваивания
	int constCounter = 0; //Счетчик констант

	std::queue<CTokenPtr> variableQueue;
	CTokenPtr lastToken;
	std::string programHeader;
	std::string programData;
	std::string programStart;
	std::string programEnd;
	
	std::map<std::string, VariantType> varTypes;

	int label = 1; //Лейбел для цикла или для ифа
	std::string whileCondition;
	std::string whileOperator;
	bool isCycle = false;


	CGenerator()
	{
		

		programHeader=
			R"(.386
			   .model flat, C
			   option casemap :none

			   include \masm32\include\msvcrt.inc
			   includelib \masm32\lib\msvcrt.lib
			   
				)";
		programData=
			R"(	.data
	             FormatInteger db '%d', 0
	             FormatDecimal db '%f', 0
				 FALSE equ 0
				 TRUE  equ
			   )";

		programStart=
			R"(	.code
				start:
			   )";

		programEnd=
			R"(
	invoke crt__exit, 0
end start
			   )";
	}



	void pushToken(CTokenPtr token)
	{
		if (generationStage == GenerationStage::Nothing)
			return;
		else if (generationStage == GenerationStage::Expression)
			expressionQueue.push(token);
		else if (generationStage == GenerationStage::AssignableVar)
			assignedIdent = token;
		else if (generationStage == GenerationStage::ExpressionEND)
		{
			generationStage = GenerationStage::Nothing;
			convertToPolandNotation();
			if (varTypes[dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue()] == VariantType::vtBoolean)
			{
				evaluateBoolAssign();
			}
			else
			{
				
				evaluatePolandNotation();
				evaluateAssign();
			}
			

		}
		else if (generationStage == GenerationStage::ExpressionLogicalEND)
		{
			generationStage = GenerationStage::Nothing;
			whileCondition += "L" + std::to_string(label)+":\n";
			
			convertToPolandNotation();
			evaluateLogicalExpression();
		}
		else if (generationStage == GenerationStage::WhileEND)
		{
			generationStage = GenerationStage::Nothing;
			programStart += "jmp L" + std::to_string(label)+"\n";
			programStart += "L" + std::to_string(label+1) + ":\n";
			programStart += whileOperator;
			programStart += whileCondition;
			label++;
			isCycle = false;
		}

	}

	void pushToVariableQueue(CTokenPtr token)
	{
		variableQueue.push(token);
	}

	void setVariablesType(CIdentToken* varType)
	{
			while (!variableQueue.empty())
			{
				programData += dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue();
				if (varType->GetValue() == "real")
				{
					programData += " real8 ?\n";
					varTypes[dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue()]= VariantType::vtReal;
				}
				else if (varType->GetValue() == "integer")
				{
					programData += " dd ?\n";
					varTypes[dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue()] = VariantType::vtInt;

				}
				else if (varType->GetValue() == "string")
				{
					programData += " db 100\n";
					programData += " dd ?\n";
					varTypes[dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue()] = VariantType::vtString;
				}
				else if (varType->GetValue() == "boolean")
				{
					programData += " dd FALSE\n";
					varTypes[dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue()] = VariantType::vtBoolean;

				}
				variableQueue.pop();
			}
	}

	int getExprPriority(CKeywordToken* val)
	{
		if (val->GetValue() == KeyWords::bracketCloseSy ||
			val->GetValue() == KeyWords::bracketOpenSy)
		{
			return 1;
		}
		else if (val->GetValue() == KeyWords::plusSy ||
			val->GetValue() == KeyWords::minusSy ||
			val->GetValue() == KeyWords::orSy)
		{
			return 2;
		}
		else
		{
			return 3;
		}
	}

	void convertToPolandNotation()
	{
		std::stack<CTokenPtr> exprS;

		while (!expressionQueue.empty())
		{
			if (dynamic_cast<CIdentToken*>(expressionQueue.front().get()))
			{
				CTokenPtr tmp = expressionQueue.front();
				exprQ.push(tmp);
				expressionQueue.pop();
			}
			else if (dynamic_cast<CConstToken*>(expressionQueue.front().get()))
			{
				CTokenPtr tmp = expressionQueue.front();
				exprQ.push(tmp);
				expressionQueue.pop();
			}
			else if (dynamic_cast<CKeywordToken*>(expressionQueue.front().get()))
			{
				CKeywordToken* tmp = dynamic_cast<CKeywordToken*>(expressionQueue.front().get());
				
				int curPriority;
				int stackPriority;
				if (exprS.empty())
				{
					exprS.push(expressionQueue.front());
				}
				else
				{
					CKeywordToken* stackTop = dynamic_cast<CKeywordToken*>(exprS.top().get());
					stackPriority = getExprPriority(stackTop);
					curPriority = getExprPriority(tmp);

					if (tmp->GetValue() == KeyWords::bracketCloseSy)
					{
						while (dynamic_cast<CKeywordToken*>(exprS.top().get())->GetValue() != KeyWords::bracketOpenSy)
						{
							exprQ.push(exprS.top());
							exprS.pop();
						}
						exprS.pop();
					}

					else if (stackPriority >= curPriority && 
						(tmp->GetValue()!=KeyWords::bracketOpenSy))
					{
						


						while (stackPriority >= curPriority && !exprS.empty())
						{
							exprQ.push(exprS.top());
							exprS.pop();
							if (!exprS.empty())
								stackPriority = getExprPriority(dynamic_cast<CKeywordToken*>(exprS.top().get()));
							else
								stackPriority = -1;
						}
						exprS.push(expressionQueue.front());
					}
					else
						exprS.push(expressionQueue.front());


				}
				expressionQueue.pop();

			}
		}

		while (!exprS.empty())
		{
			exprQ.push(exprS.top());
			exprS.pop();
		}
		
	}

	void evaluatePolandNotation()
	{
		while (!exprQ.empty())
		{
			CTokenPtr tmp= exprQ.front();
			

			if (dynamic_cast<CConstToken*>(tmp.get()))
			{
				CConstToken* constTok = dynamic_cast<CConstToken*>(tmp.get());
				constCounter++;
				//constQ.push("const" + std::to_string(constCounter));

				if (constTok->GetConstType() == VariantType::vtInt)
				{
					programData += "const" + std::to_string(constCounter)+ " dd "+constTok->ToString()+"\n";
					if (!isCycle)
						programStart += "fild const"+ std::to_string(constCounter) + "\n";
					else
						whileOperator+= "fild const" + std::to_string(constCounter) + "\n";
				}
				else if (constTok->GetConstType() == VariantType::vtReal)
				{
					programData += "const" + std::to_string(constCounter) + " real8 " + constTok->ToString() + "\n";
					if (!isCycle)
						programStart += "fld const" + std::to_string(constCounter)+"\n";
					else
						whileOperator += "fld const" + std::to_string(constCounter) + "\n";
				}
				else if (constTok->GetConstType() == VariantType::vtBoolean)
				{
					programData += "const" + std::to_string(constCounter) + " dd " + constTok->ToString() + "\n";
				}
				else if (constTok->GetConstType() == VariantType::vtString)
				{
					programData += "const" + std::to_string(constCounter) + " db \"" + constTok->ToString() + "\",0\n";
				}
				
				
			}
			else if (dynamic_cast<CKeywordToken*>(tmp.get()))
			{
				CKeywordToken* keywordTok = dynamic_cast<CKeywordToken*>(tmp.get());
				
				if (keywordTok->GetValue() == KeyWords::multiplySy)
				{
					if (!isCycle)
						programStart += "fmul \n";
					else
						whileOperator += "fmul \n";
				}
				else if (keywordTok->GetValue() == KeyWords::divisionSy)
				{
					if (!isCycle)
						programStart += "fdiv \n";
					else
						whileOperator += "fdiv \n";
				}
				else if (keywordTok->GetValue() == KeyWords::plusSy)
				{
					if (!isCycle)
						programStart += "fadd \n";
					else
						whileOperator += "fadd \n";
				}
				else if (keywordTok->GetValue() == KeyWords::minusSy)
				{
					if (!isCycle)
						programStart += "fsub \n";
					else
						whileOperator += "fsub \n";

				}





			}


			exprQ.pop();
		}
	}

	void evaluateLogicalExpression()
	{
		int boolFound = -1; //0-false, 1- true
		int stackObserved = 0;

		while (!exprQ.empty())
		{
			if (dynamic_cast<CIdentToken*>(exprQ.front().get()))
			{
				CIdentToken* tokenIdent = dynamic_cast<CIdentToken*>(exprQ.front().get());
				if (varTypes[tokenIdent->GetValue()] == VariantType::vtInt)
				{
					whileCondition += "fild " + tokenIdent->GetValue() + "\n";
				}
				else if (varTypes[tokenIdent->GetValue()] == VariantType::vtReal)
				{
					whileCondition += "fld " + tokenIdent->GetValue() + "\n";
				}
			}
			else if (dynamic_cast<CConstToken*>(exprQ.front().get()))
			{
				constCounter++;
				CConstToken* tokenConst = dynamic_cast<CConstToken*>(exprQ.front().get());

				if (tokenConst->GetConstType() == VariantType::vtInt)
				{
					programData += "const" + std::to_string(constCounter) + " dd " + tokenConst->ToString() + "\n";
					whileCondition += "fild " + tokenConst->ToString() + "\n";
				}
				else if (tokenConst->GetConstType() == VariantType::vtReal)
				{
					programData += "const" + std::to_string(constCounter) + " real8 " + tokenConst->ToString() + "\n";
					whileCondition += "fld " + tokenConst->ToString() + "\n";
				}
				else if (tokenConst->GetConstType() == VariantType::vtBoolean)
				{
					if (tokenConst->ToString() == "false")
					{
						boolFound = 0;
					}
					else if (tokenConst->ToString() == "true")
					{
						boolFound = 1;
					}
				}
				
			}

			else if (dynamic_cast<CKeywordToken*>(exprQ.front().get()))
			{
				CKeywordToken* keywordTok = dynamic_cast<CKeywordToken*>(exprQ.front().get());
				whileCondition += "fcompp\n";
				if (keywordTok->GetValue() == KeyWords::equalSy)
				{
					if (boolFound == 0)
						whileCondition += "jne L" + std::to_string(label+1) +"\n";
					else
						whileCondition += "je L" + std::to_string(label+1) + "\n";
				}
				else if (keywordTok->GetValue() == KeyWords::notEqualSy)
				{
					if (boolFound == 0)
						whileCondition += "je L" + std::to_string(label + 1) + "\n";
					else
						whileCondition += "jne L" + std::to_string(label + 1) + "\n";
				}
				else if (keywordTok->GetValue() == KeyWords::moreSy)
				{
					if (boolFound == 0)
						whileCondition += "jle L" + std::to_string(label + 1) + "\n";
					else
						whileCondition += "jm L" + std::to_string(label + 1) + "\n";

				}
				else if (keywordTok->GetValue() == KeyWords::moreEqualSy)
				{
					if (boolFound == 0)
						whileCondition += "jl L" + std::to_string(label + 1) + "\n";
					else
						whileCondition += "jme L" + std::to_string(label + 1) + "\n";
				}
				else if (keywordTok->GetValue() == KeyWords::lessSy)
				{
					if (boolFound == 0)
						whileCondition += "jme L" + std::to_string(label + 1) + "\n";
					else
						whileCondition += "jl L" + std::to_string(label + 1) + "\n";
				}
				else if (keywordTok->GetValue() == KeyWords::lessEqualSy)
				{
					if (boolFound == 0)
						whileCondition += "jm L" + std::to_string(label + 1) + "\n";
					else
						whileCondition += "jle L" + std::to_string(label + 1) + "\n";

				}

			}
			
			exprQ.pop();
			stackObserved++;
		}

		if (boolFound == 0 && stackObserved == 1)
		{
			whileCondition += "nop \n";
		}
		else if (boolFound == 1 && stackObserved == 1)
		{
			whileCondition += "jmp L"+std::to_string(label+1) + "\n";
		}
	}

	void evaluateAssign()
	{
		//ПРОВЕРЯТЬ int ИЛИ REAL

		std::string tmp = dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue();
		if (!isCycle)
			programStart += "fstp " + tmp + "\n";
		else
			whileOperator+= "fstp " + tmp + "\n";
	}

	void evaluateBoolAssign()
	{
		CTokenPtr tmp = exprQ.front();
		if (dynamic_cast<CConstToken*>(tmp.get())->ToString() == "false")
		{
			if (!isCycle)
				programStart += "MOV eax,FALSE\n";
			else
				whileOperator+= "MOV eax,FALSE\n";

		}
		else
		{
			if(!isCycle)
				programStart += "MOV eax,TRUE\n";
			else
				whileOperator += "MOV eax,TRUE\n";

		}
		if (!isCycle)
			programStart += "MOV " + dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue() + ",eax\n";
		else
			whileOperator += "MOV " + dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue() + ",eax\n";

		exprQ.pop();
	}

	void changeGeneratorStage(GenerationStage gs)
	{
		generationStage = gs;
		if (gs == GenerationStage::WhileSTART)
		{
			isCycle = true;
		}
		else if (gs == GenerationStage::WhileEND)
		{
			isCycle = false;
		}
	}


	//void makeTreeFromPolandNotation()
	//{
	//	std::stack<TreeNodePtr> treeStack;
	//	

	//	while (!exprQ.empty())
	//	{
	//		CTokenPtr q = exprQ.front();

	//		if (dynamic_cast<CKeywordToken*>(q.get()))
	//		{
	//			//ДОБАВИТЬ УНАРНЫЙ NOT
	//			TreeNodePtr newElem = std::make_shared<TreeNode>();
	//			newElem.get()->tok = q;
	//			newElem.get()->childL = treeStack.top();
	//			treeStack.pop();
	//			newElem.get()->childR = treeStack.top();
	//			treeStack.pop();

	//			root = newElem;
	//			treeStack.push(newElem);
	//		}
	//		else
	//		{
	//			TreeNodePtr newElem = std::make_shared<TreeNode>();
	//			newElem.get()->tok = q;
	//			newElem.get()->constant = constQ.front();
	//			constQ.pop();
	//			treeStack.push(newElem);
	//			
	//		}





	//		exprQ.pop();

	//	}
	//}








	void printCodeToFile()
	{
		std::ofstream out;          // поток для записи
		out.open("C:\\Users\\jdczy\\source\\repos\\Compilator\\program.asm"); // окрываем файл для записи
		if (out.is_open())
		{
			out << programHeader << std::endl;
			out << programData << std::endl;
			out << programStart << std::endl;
			out << programEnd << std::endl;
		}

		std::cout << "End of program. Creating .obj file and exe..." << std::endl;
		//WinExec("D:\\Clear.bat", SW_SHOW);
		
		//std::system("cmd.exe /C C:\\Users\\jdczy\\source\\repos\\Compilator\\execute1.bat");
		//Sleep(2000);
		//std::system("cmd.exe /C C:\\Users\\jdczy\\source\\repos\\Compilator\\execute2.bat");
	}





	bool isInteger(std::string str)
	{

	}
};