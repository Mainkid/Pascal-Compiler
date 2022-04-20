#include "CGenerator.h"



IfExp::IfExp()
{

}

IfExp::IfExp(int label)
{
	ifCondition = "";
	thenOperator = "";
	elseOperator = "";
	labelCount = label;
}

WhileExp::WhileExp()
{
	whileCondition = "";
	whileOperator = "";
}

WhileExp::WhileExp(int label)
{
	whileCondition = "";
	whileOperator = "";
	labelCount = label;
}

CGenerator::CGenerator(bool isWorking)
{
	if (!isWorking)
		generationStage = GenerationStage::Error;

	programHeader =
		R"(.386
			   .model flat, C
			   option casemap :none

			   include \masm32\include\msvcrt.inc
			   includelib \masm32\lib\msvcrt.lib
			   
				)";
	programData =
		R"(	.data
	             FormatInteger db '%d ', 0
	             FormatDecimal db '%f ', 0
				 FALSE equ 0
				 TRUE  equ
			   )";

	programStart =
		R"(	.code
				start:
			   )";

	programEnd =
		R"(
	invoke crt__exit, 0
end start
			   )";
}

void CGenerator::WriteLN()
{
	//Проверить, куда выводить
	if (dynamic_cast<CIdentToken*>(WriteLNToken.get()))
	{
		CIdentToken* tmp = dynamic_cast<CIdentToken*>(WriteLNToken.get());
		if (varTypes[tmp->GetValue()] == VariantType::vtInt)
		{
			//ЗДЕСЬ
			if (whileStack.empty())
				programStart += "invoke crt_printf, addr FormatInteger, " + tmp->GetValue() + "\n";
			else
				whileStack.top()->whileOperator += "invoke crt_printf, addr FormatInteger, " + tmp->GetValue() + "\n";
		}
		else if (varTypes[tmp->GetValue()] == VariantType::vtReal)
		{
			if (whileStack.empty())
				programStart += "invoke crt_printf, addr FormatDecimal, " + tmp->GetValue() + "\n";
			else
				whileStack.top()->whileOperator += "invoke crt_printf, addr FormatDecimal, " + tmp->GetValue() + "\n";

		}

	}
	else if (dynamic_cast<CConstToken*>(WriteLNToken.get()))
	{
		constCounter++;
		CConstToken* tmp = dynamic_cast<CConstToken*>(WriteLNToken.get());
		if (tmp->GetConstType() == VariantType::vtInt)
		{
			if (whileStack.empty())
				programStart += "invoke crt_printf, addr FormatInteger, const" + std::to_string(constCounter) + "\n";
			else
				whileStack.top()->whileOperator += "invoke crt_printf, addr FormatInteger, const" + std::to_string(constCounter) + "\n";
			programData += "const" + std::to_string(constCounter) + " dd " + tmp->ToString() + "\n";
		}
		else if (tmp->GetConstType() == VariantType::vtReal)
		{
			if (whileStack.empty())
				programStart += "invoke crt_printf, addr FormatDecimal, const" + std::to_string(constCounter) + "\n";
			else
				whileStack.top()->whileOperator += "invoke crt_printf, addr FormatDecimal, const" + std::to_string(constCounter) + "\n";
			programData += "const" + std::to_string(constCounter) + " real8 " + tmp->ToString() + "\n";
		}
		else if (tmp->GetConstType() == VariantType::vtBoolean)
		{

			programData += "const" + std::to_string(constCounter) + " dd " + tmp->ToString() + "\n";
		}
		else if (tmp->GetConstType() == VariantType::vtString)
		{
			programData += "const" + std::to_string(constCounter) + " db \"" + tmp->ToString() + "\",0\n";
		}



	}
}

void CGenerator::pushToken(CTokenPtr token)
{
	if (generationStage == GenerationStage::Error)
		return;


	if (generationStage == GenerationStage::Nothing)
		return;
	else if (generationStage == GenerationStage::Expression)
		expressionQueue.push(token);
	else if (generationStage == GenerationStage::AssignableVar)
		assignedIdent = token;
	else if (generationStage == GenerationStage::WRITELN)
	{
		WriteLNToken = token;
		generationStage = GenerationStage::Nothing;
		WriteLN();

	}
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

			evaluatePolandNotation(false);
			evaluateAssign();
		}


	}
	else if (generationStage == GenerationStage::ExpressionIfEnd)
	{
		//См ниже

	}
	else if (generationStage == GenerationStage::ExpressionLogicalEND)
	{
		generationStage = GenerationStage::Nothing;
		whileStack.top().get()->whileCondition += "L" + std::to_string(whileStack.top().get()->labelCount) + ":\n";

		convertToPolandNotation();
		evaluateLogicalExpression(false);
	}
	else if (generationStage == GenerationStage::ExpressionLogicalIFEND)
	{
		generationStage = GenerationStage::Nothing;
		convertToPolandNotation();
		evaluateLogicalExpression(true);
	}


	else if (generationStage == GenerationStage::WhileEND)
	{
		generationStage = GenerationStage::Nothing;


		WhilePtr wp = whileStack.top();
		whileStack.pop();

		if (whileStack.empty() && ifStack.empty())
		{
			programStart += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
			programStart += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
			//Проверить, в начало программы, в if или в while
			programStart += wp.get()->whileOperator;
			programStart += wp.get()->whileCondition;
		}
		else if (!whileStack.empty() && ifStack.empty())
		{
			whileStack.top()->whileOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
			whileStack.top()->whileOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
			whileStack.top()->whileOperator += wp.get()->whileOperator;
			whileStack.top()->whileOperator += wp.get()->whileCondition;
		}
		else if (whileStack.empty() && !ifStack.empty())
		{
			if (branch == IfBranch::Then)
			{
				ifStack.top()->thenOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
				ifStack.top()->thenOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
				ifStack.top()->thenOperator += wp.get()->whileOperator;
				ifStack.top()->thenOperator += wp.get()->whileCondition;
			}
			else if (branch == IfBranch::Else)
			{
				ifStack.top()->elseOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
				ifStack.top()->elseOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
				ifStack.top()->elseOperator += wp.get()->whileOperator;
				ifStack.top()->elseOperator += wp.get()->whileCondition;
			}
		}


		else if (!whileStack.empty() && !ifStack.empty())
		{
			if (whileStack.top()->labelCount > ifStack.top()->labelCount)
			{
				whileStack.top()->whileOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
				whileStack.top()->whileOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
				whileStack.top()->whileOperator += wp.get()->whileOperator;
				whileStack.top()->whileOperator += wp.get()->whileCondition;
			}
			else
			{
				if (branch == IfBranch::Then)
				{
					ifStack.top()->thenOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
					ifStack.top()->thenOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
					ifStack.top()->thenOperator += wp.get()->whileOperator;
					ifStack.top()->thenOperator += wp.get()->whileCondition;
				}
				else if (branch == IfBranch::Else)
				{
					ifStack.top()->elseOperator += "jmp L" + std::to_string(wp.get()->labelCount) + "\n";
					ifStack.top()->elseOperator += "L" + std::to_string(wp.get()->labelCount + 1) + ":\n";
					ifStack.top()->elseOperator += wp.get()->whileOperator;
					ifStack.top()->elseOperator += wp.get()->whileCondition;
				}
			}
		}


		isCycle = false;
	}

	

}

void CGenerator::pushToVariableQueue(CTokenPtr token)
{
	variableQueue.push(token);
}

void CGenerator::setVariablesType(CIdentToken* varType)
{
	while (!variableQueue.empty())
	{
		programData += dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue();
		if (varType->GetValue() == "real")
		{
			programData += " real8 ?\n";
			varTypes[dynamic_cast<CIdentToken*>(variableQueue.front().get())->GetValue()] = VariantType::vtReal;
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

int CGenerator::getExprPriority(CKeywordToken* val)
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

void CGenerator::convertToPolandNotation()
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
					(tmp->GetValue() != KeyWords::bracketOpenSy))
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

void CGenerator::evaluatePolandNotation(bool isIf)
{
	while (!exprQ.empty())
	{
		CTokenPtr tmp = exprQ.front();


		if (dynamic_cast<CConstToken*>(tmp.get()))
		{
			CConstToken* constTok = dynamic_cast<CConstToken*>(tmp.get());
			constCounter++;
			//constQ.push("const" + std::to_string(constCounter));

			if (constTok->GetConstType() == VariantType::vtInt)
			{
				programData += "const" + std::to_string(constCounter) + " dd " + constTok->ToString() + "\n";
				if (ifStack.empty() && whileStack.empty())
					programStart += "fild const" + std::to_string(constCounter) + "\n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fild const" + std::to_string(constCounter) + "\n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fild const" + std::to_string(constCounter) + "\n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fild const" + std::to_string(constCounter) + "\n";
				}
			}
			else if (constTok->GetConstType() == VariantType::vtReal)
			{
				programData += "const" + std::to_string(constCounter) + " real8 " + constTok->ToString() + "\n";
				if (ifStack.empty() && whileStack.empty())
					programStart += "fld const" + std::to_string(constCounter) + "\n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fld const" + std::to_string(constCounter) + "\n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fld const" + std::to_string(constCounter) + "\n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fld const" + std::to_string(constCounter) + "\n";
				}
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
		else if (dynamic_cast<CIdentToken*>(tmp.get()))
		{
			CIdentToken* cit = dynamic_cast<CIdentToken*>(tmp.get());

			if (varTypes[cit->GetValue()] == VariantType::vtInt)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fild " + cit->GetValue() + "\n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fild " + cit->GetValue() + "\n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fild " + cit->GetValue() + "\n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fild " + cit->GetValue() + "\n";


				}
			}
			else if (varTypes[cit->GetValue()] == VariantType::vtReal)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fld " + cit->GetValue() + "\n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fld " + cit->GetValue() + "\n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fld " + cit->GetValue() + "\n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fld " + cit->GetValue() + "\n";

				}

			}



		}




		else if (dynamic_cast<CKeywordToken*>(tmp.get()))
		{
			CKeywordToken* keywordTok = dynamic_cast<CKeywordToken*>(tmp.get());

			if (keywordTok->GetValue() == KeyWords::multiplySy)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fmul \n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fmul \n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fmul \n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fmul \n";


				}
			}
			else if (keywordTok->GetValue() == KeyWords::divisionSy)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fdiv \n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fdiv \n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fdiv \n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fdiv \n";

				}
			}
			else if (keywordTok->GetValue() == KeyWords::plusSy)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fadd \n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fadd \n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fadd \n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fadd \n";

				}
			}
			else if (keywordTok->GetValue() == KeyWords::minusSy)
			{
				if (!isIf && whileStack.empty() || isIf && ifStack.empty())
					programStart += "fsub \n";
				else if (!isIf && !whileStack.empty())
					whileStack.top().get()->whileOperator += "fsub \n";
				else if (isIf && !ifStack.empty())
				{
					if (branch == IfBranch::Then)
						ifStack.top().get()->thenOperator += "fsub \n";
					else if (branch == IfBranch::Else)
						ifStack.top().get()->elseOperator += "fsub \n";

				}

			}





		}


		exprQ.pop();
	}
}

void CGenerator::evaluateLogicalExpression(bool isIf)
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

				if (!isIf)
					whileStack.top().get()->whileCondition += "fild " + tokenIdent->GetValue() + "\n";
				else
					ifStack.top().get()->ifCondition += "fild " + tokenIdent->GetValue() + "\n";
			}
			else if (varTypes[tokenIdent->GetValue()] == VariantType::vtReal)
			{
				if (!isIf)
					whileStack.top().get()->whileCondition += "fld " + tokenIdent->GetValue() + "\n";
				else
					ifStack.top().get()->ifCondition += "fld " + tokenIdent->GetValue() + "\n";
			}
		}
		else if (dynamic_cast<CConstToken*>(exprQ.front().get()))
		{
			constCounter++;
			CConstToken* tokenConst = dynamic_cast<CConstToken*>(exprQ.front().get());

			if (tokenConst->GetConstType() == VariantType::vtInt)
			{
				programData += "const" + std::to_string(constCounter) + " dd " + tokenConst->ToString() + "\n";
				if (!isIf)
					whileStack.top().get()->whileCondition += "fild const" + std::to_string(constCounter) + "\n";
				else
					ifStack.top().get()->ifCondition += "fild const" + std::to_string(constCounter) + "\n";
			}
			else if (tokenConst->GetConstType() == VariantType::vtReal)
			{
				programData += "const" + std::to_string(constCounter) + " real8 " + tokenConst->ToString() + "\n";
				if (!isIf)
					whileStack.top().get()->whileCondition += "fld const" + std::to_string(constCounter) + "\n";
				else
					ifStack.top().get()->ifCondition += "fld const" + std::to_string(constCounter) + "\n";
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
			if (!isIf)
			{
				whileStack.top().get()->whileCondition += "fcompp\n";
				whileStack.top().get()->whileCondition += "fstsw ax\n";
				whileStack.top().get()->whileCondition += "sahf\n";
			}
			else
			{
				ifStack.top().get()->ifCondition += "fcompp\n";
				ifStack.top().get()->ifCondition += "fstsw ax\n";
				ifStack.top().get()->ifCondition += "sahf\n";
			}
			if (keywordTok->GetValue() == KeyWords::equalSy)
			{
				if (boolFound == 0)
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jne L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "je L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "je L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jne L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
			}
			else if (keywordTok->GetValue() == KeyWords::notEqualSy)
			{
				if (boolFound == 0)

				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "je L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jne L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jne L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "je L" + std::to_string(ifStack.top().get()->labelCount) + "\n";

				}
			}
			else if (keywordTok->GetValue() == KeyWords::moreSy)
			{
				if (boolFound == 0)
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jae L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jb L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jb L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jae L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
			}
			else if (keywordTok->GetValue() == KeyWords::moreEqualSy)
			{
				if (boolFound == 0)
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "ja L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jbe L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jbe L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "ja L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
			}
			else if (keywordTok->GetValue() == KeyWords::lessSy)
			{
				if (boolFound == 0)
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jbe L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "ja L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "ja L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jbe L" + std::to_string(ifStack.top().get()->labelCount) + "\n";
				}
			}
			else if (keywordTok->GetValue() == KeyWords::lessEqualSy)
			{
				if (boolFound == 0)
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jb L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "ja L" + std::to_string(ifStack.top().get()->labelCount) + "\n";

				}
				else
				{
					if (!isIf)
						whileStack.top().get()->whileCondition += "jae L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
					else
						ifStack.top().get()->ifCondition += "jb L" + std::to_string(ifStack.top().get()->labelCount) + "\n";

				}
			}

		}

		exprQ.pop();
		stackObserved++;
	}

	if (boolFound == 0 && stackObserved == 1)
	{
		if (!isIf)
			whileStack.top().get()->whileCondition += "nop \n";
		else
			ifStack.top().get()->ifCondition += "nop \n";
	}
	else if (boolFound == 1 && stackObserved == 1)
	{
		if (!isIf)
			whileStack.top().get()->whileCondition += "jmp L" + std::to_string(whileStack.top().get()->labelCount + 1) + "\n";
		else
			ifStack.top().get()->ifCondition += "jmp L" + std::to_string(whileStack.top().get()->labelCount) + "\n";

	}
}

void CGenerator::evaluateIfLogicalExpression()
{

}

void CGenerator::evaluateAssign()
{

	std::string tmp = dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue();

	if (varTypes[tmp] == VariantType::vtReal)
	{
		if (whileStack.empty() && ifStack.empty())
			programStart += "fstp " + tmp + "\n";
		else if (!whileStack.empty() && ifStack.empty() || (!whileStack.empty() && !ifStack.empty() && whileStack.top()->labelCount > ifStack.top()->labelCount))
			whileStack.top().get()->whileOperator += "fstp " + tmp + "\n";
		else if (!ifStack.empty() && whileStack.empty() || (!whileStack.empty() && !ifStack.empty() && whileStack.top()->labelCount > ifStack.top()->labelCount))
		{
			if (branch == IfBranch::Then)
			{
				ifStack.top()->thenOperator += "fstp " + tmp + "\n";
			}
			else if (branch == IfBranch::Else)
			{
				ifStack.top()->elseOperator += "fstp " + tmp + "\n";
			}
		}
	}
	else
	{
		if (whileStack.empty() && ifStack.empty())
			programStart += "fistp " + tmp + "\n";
		else if (!whileStack.empty() && ifStack.empty() || (!whileStack.empty() && !ifStack.empty() && whileStack.top()->labelCount > ifStack.top()->labelCount))
			whileStack.top().get()->whileOperator += "fistp " + tmp + "\n";
		else if (!ifStack.empty() && whileStack.empty() || (!whileStack.empty() && !ifStack.empty() && whileStack.top()->labelCount < ifStack.top()->labelCount))
		{
			if (branch == IfBranch::Then)
			{
				ifStack.top()->thenOperator += "fistp " + tmp + "\n";
			}
			else if (branch == IfBranch::Else)
			{
				ifStack.top()->elseOperator += "fistp " + tmp + "\n";
			}


		}
	}
}

void CGenerator::evaluateBoolAssign()
{
	CTokenPtr tmp = exprQ.front();
	if (dynamic_cast<CConstToken*>(tmp.get())->ToString() == "false")
	{
		if (!isCycle)
			programStart += "MOV eax,FALSE\n";
		else
			whileStack.top().get()->whileOperator += "MOV eax,FALSE\n";

	}
	else
	{
		if (!isCycle)
			programStart += "MOV eax,TRUE\n";
		else
			whileStack.top().get()->whileOperator += "MOV eax,TRUE\n";

	}
	if (!isCycle)
		programStart += "MOV " + dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue() + ",eax\n";
	else
		whileStack.top().get()->whileOperator += "MOV " + dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue() + ",eax\n";

	exprQ.pop();
}

void CGenerator::changeGeneratorStage(GenerationStage gs)
{
	if (generationStage == GenerationStage::Error)
		return;
	generationStage = gs;
	if (gs == GenerationStage::WhileSTART)
	{
		isCycle = true;
		label += 2;
		WhilePtr wp = std::make_shared<WhileExp>(label);
		whileStack.push(wp);
	}
	else if (gs == GenerationStage::WhileEND)
	{
		isCycle = false;
	}
	else if (gs == GenerationStage::IFSTART)
	{
		label += 2;
		IfPtr ip = std::make_shared<IfExp>(label);
		ifStack.push(ip);
	}
	else if (gs == GenerationStage::THENSTART)
	{
		branch = IfBranch::Then;
	}
	else if (gs == GenerationStage::ELSESTART)
	{
		branch = IfBranch::Else;
	}

	else if (gs == GenerationStage::ExpressionIfEnd)
	{
		generationStage = GenerationStage::Nothing;
		convertToPolandNotation();
		if (varTypes[dynamic_cast<CIdentToken*>(assignedIdent.get())->GetValue()] == VariantType::vtBoolean)
		{
			evaluateBoolAssign();
		}
		else
		{
			if (exprQ.size() > 0)
			{
				evaluatePolandNotation(true);
				evaluateAssign();
			}
		}
	}
	else if (gs == GenerationStage::IfEND)
	{
		generationStage = GenerationStage::Nothing;
		IfPtr ip = ifStack.top();
		ifStack.pop();
		ip.get()->thenOperator += "jmp L" + std::to_string(ip.get()->labelCount + 1) + "\n";
		ip.get()->thenOperator = ip.get()->thenOperator + "L" + std::to_string(ip.get()->labelCount) + ":\n";

		ip.get()->elseOperator = ip.get()->elseOperator + "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";

		if (whileStack.empty() && ifStack.empty())
		{

			programStart += ip.get()->ifCondition;
			programStart += ip.get()->thenOperator;
			programStart += ip.get()->elseOperator;
		}
		else if (!whileStack.empty() && ifStack.empty())
		{
			//whileStack.top()->whileOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
			//whileStack.top()->whileOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
			whileStack.top()->whileOperator += ip.get()->ifCondition;
			whileStack.top()->whileOperator += ip.get()->thenOperator;
			whileStack.top()->whileOperator += ip.get()->elseOperator;
		}
		else if (whileStack.empty() && !ifStack.empty())
		{
			if (branch == IfBranch::Then)
			{
				//ifStack.top()->thenOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
				//ifStack.top()->thenOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
				ifStack.top()->thenOperator += ip.get()->ifCondition;
				ifStack.top()->thenOperator += ip.get()->thenOperator;
				ifStack.top()->thenOperator += ip.get()->elseOperator;
			}
			else if (branch == IfBranch::Else)
			{
				//ifStack.top()->elseOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
				//ifStack.top()->elseOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
				ifStack.top()->elseOperator += ip.get()->ifCondition;
				ifStack.top()->elseOperator += ip.get()->thenOperator;
				ifStack.top()->elseOperator += ip.get()->elseOperator;
			}

		}
		else if (!whileStack.empty() && !ifStack.empty())
		{
			if (whileStack.top()->labelCount > ifStack.top()->labelCount)
			{
				//whileStack.top()->whileOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
				//whileStack.top()->whileOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
				whileStack.top()->whileOperator += ip.get()->ifCondition;
				whileStack.top()->whileOperator += ip.get()->thenOperator;
				whileStack.top()->whileOperator += ip.get()->elseOperator;
			}
			else
			{
				if (branch == IfBranch::Then)
				{
					//ifStack.top()->thenOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
					//ifStack.top()->thenOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
					ifStack.top()->thenOperator += ip.get()->ifCondition;
					ifStack.top()->thenOperator += ip.get()->thenOperator;
					ifStack.top()->thenOperator += ip.get()->elseOperator;
				}
				else if (branch == IfBranch::Else)
				{
					//ifStack.top()->elseOperator += "jmp L" + std::to_string(ip.get()->labelCount) + "\n";
					//ifStack.top()->elseOperator += "L" + std::to_string(ip.get()->labelCount + 1) + ":\n";
					ifStack.top()->elseOperator += ip.get()->ifCondition;
					ifStack.top()->elseOperator += ip.get()->thenOperator;
					ifStack.top()->elseOperator += ip.get()->elseOperator;
				}
			}
		}
	}
}

void CGenerator::printCodeToFile()
{
	if (generationStage == GenerationStage::Error)
		return;


	std::ofstream out;          // поток для записи
	out.open("C:\\Users\\jdczy\\source\\repos\\Compilator\\program.asm"); // окрываем файл для записи
	if (out.is_open())
	{
		out << programHeader << std::endl;
		out << programData << std::endl;
		out << programStart << std::endl;
		out << programEnd << std::endl;
	}
	out.close();
	std::cout << "End of program. Creating .obj file and exe..." << std::endl;
	//WinExec("D:\\Clear.bat", SW_SHOW);

	std::system("C:\\Users\\jdczy\\source\\repos\\Compilator\\execute.bat");
	//Sleep(4000);
	//std::system("cmd.exe /C C:\\Users\\jdczy\\source\\repos\\Compilator\\execute2.bat");
}

bool CGenerator::isInteger(std::string str)
{
	return false;
}