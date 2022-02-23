#include <string>
#include <string.h>
#include <cstring>
#include <memory>
#include "CVariant.h"



CBooleanVariant::CBooleanVariant(bool val) : CVariant(VariantType::vtBoolean)
{
	value = val;
}

CRealVariant::CRealVariant(float val) : CVariant(VariantType::vtBoolean)
{
	value = val;
}

CIntVariant::CIntVariant(int val) : CVariant(VariantType::vtInt)
{
	value = val;
}

CStringVariant::CStringVariant(std::string val) : CVariant(VariantType::vtString)
{
	value = val;
}

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

	return std::to_string(value); 
}

float CRealVariant::GetValue()
{
	return value;
}

std::string CRealVariant::ToString()
{
	return std::to_string(value); 
}

bool CBooleanVariant::GetValue()
{
	return value;
}

std::string CBooleanVariant::ToString()
{
	return std::to_string(value); 
}

std::string CStringVariant::GetValue()
{
	return value;
}

std::string CStringVariant::ToString()
{
	return value;
}