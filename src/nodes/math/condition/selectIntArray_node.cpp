#include "selectIntArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectIntArray::SelectIntArray() {}
SelectIntArray::~SelectIntArray() {}

// ------ Attr ------
MObject SelectIntArray::ifFalseAttr;
MObject SelectIntArray::ifTrueAttr;
MObject SelectIntArray::conditionAttr;
MObject SelectIntArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectIntArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectIntArray::initialize()
{
	std::vector<int> intArray;

	createIntDataArrayAttribute(ifFalseAttr, "ifFalse", "ifFalse", intArray, kDefaultPreset | kKeyable);
	createIntDataArrayAttribute(ifTrueAttr, "ifTrue", "ifTrue", intArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createIntDataArrayAttribute(outputAttr, "output", "output", intArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectIntArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> ifFalse = inputIntDataArrayValue(dataBlock, ifFalseAttr);
	std::vector<int> ifTrue = inputIntDataArrayValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputIntDataArrayValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------