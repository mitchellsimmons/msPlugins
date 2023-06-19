#include "conditionInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConditionInt::ConditionInt() {}
ConditionInt::~ConditionInt() {}

// ------ Attr ------
MObject ConditionInt::input1Attr;
MObject ConditionInt::input2Attr;
MObject ConditionInt::operatorAttr;
MObject ConditionInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ConditionInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ConditionInt::initialize()
{
	std::unordered_map<const char*, short> operatorFields{ {"Equal", 0}, {"Not Equal", 1}, {"Greater", 2}, {"Greater or Equal", 3}, {"Less", 4}, {"Less or Equal", 5} };
	createIntAttribute(input1Attr, "input1", "input1", 0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 0, kDefaultPreset | kKeyable);
	createEnumAttribute(operatorAttr, "operator", "operator", operatorFields, 0, kDefaultPreset | kKeyable);
	createBoolAttribute(outputAttr, "output", "output", true, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(operatorAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(operatorAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ConditionInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input1 = inputIntValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);
	short op = inputEnumValue(dataBlock, operatorAttr);

	bool output = false;
	if (op == 0)
		output = input1 == input2;
	else if (op == 1)
		output = input1 != input2;
	else if (op == 2)
		output = input1 > input2;
	else if (op == 3)
		output = input1 >= input2;
	else if (op == 4)
		output = input1 < input2;
	else if (op == 5)
		output = input1 <= input2;

	outputBoolValue(dataBlock, outputAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------