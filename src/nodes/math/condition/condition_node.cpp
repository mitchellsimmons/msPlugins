#include "condition_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Condition::Condition() {}
Condition::~Condition() {}

// ------ Attr ------
MObject Condition::input1Attr;
MObject Condition::input2Attr;
MObject Condition::operatorAttr;
MObject Condition::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Condition::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Condition::initialize()
{
	std::unordered_map<const char*, short> operatorFields{ {"Equal", 0}, {"Not Equal", 1}, {"Greater", 2}, {"Greater or Equal", 3}, {"Less", 4}, {"Less or Equal", 5} };
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
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

MStatus Condition::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);
	short op = inputEnumValue(dataBlock, operatorAttr);

	bool output = false;
	if (op == 0)
		output = MRS::isEqual(input1, input2);
	else if (op == 1)
		output = !MRS::isEqual(input1, input2);
	else if (op == 2)
		output = input1 > input2;
	else if (op == 3)
		output = input1 > input2 || MRS::isEqual(input1, input2);
	else if (op == 4)
		output = input1 < input2;
	else if (op == 5)
		output = input1 < input2 || MRS::isEqual(input1, input2);

	outputBoolValue(dataBlock, outputAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------