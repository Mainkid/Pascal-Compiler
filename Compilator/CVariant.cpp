#include <string>
#include <string.h>
#include <cstring>
#include <memory>
#include "CVariant.h"

CVariant::CVariant(VariantType vt)
{
	variantType = vt;
}

VariantType CVariant::GetVariantType()
{
	return variantType;
}

int CIntVariant::GetValue()
{
	return value;
}

std::string CIntVariant::ToString()
{

	return "to_str"; //TODO: �������� ���������� � ������
}

float CRealVariant::GetValue()
{
	return value;
}

std::string CRealVariant::ToString()
{
	return "to_str"; //TODO: �������� ���������� � ������
}

bool CBooleanVariant::GetValue()
{
	return value;
}

std::string CBooleanVariant::ToString()
{
	return "to_str"; //TODO: �������� ���������� � ������
}