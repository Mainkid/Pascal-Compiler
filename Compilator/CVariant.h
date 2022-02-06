#pragma once
#include <string>



enum class VariantType {
	vtInt, vtReal, vtString, vtBoolean
};

class CVariant {
private:
	VariantType variantType;
public:
	CVariant(VariantType);
	VariantType GetVariantType();
	virtual std::string ToString() = 0;
};

class CIntVariant : public CVariant {
private:
	int value;
public:
	CIntVariant(int val) : CVariant(VariantType::vtInt)
	{
		value = val;
	};
	int GetValue();
	std::string ToString() override;
};

class CRealVariant : public CVariant {
private:
	float value;
public:
	CRealVariant(float val) : CVariant(VariantType::vtReal)
	{
		value = val;
	};
	float GetValue();
	std::string ToString()override;

};

class CBooleanVariant: public CVariant{
private:
	bool value;
public:
	CBooleanVariant(bool val) : CVariant(VariantType::vtBoolean)
	{
		value = val;
	};
	bool GetValue();
	std::string ToString() override;
};
