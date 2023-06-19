#include "selectDoubleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectDoubleArray::SelectDoubleArray() {}
SelectDoubleArray::~SelectDoubleArray() {}

// ------ Attr ------
MObject SelectDoubleArray::ifFalseAttr;
MObject SelectDoubleArray::ifTrueAttr;
MObject SelectDoubleArray::conditionAttr;
MObject SelectDoubleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectDoubleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectDoubleArray::initialize()
{
	std::vector<double> doubleArray;

	createDoubleDataArrayAttribute(ifFalseAttr, "ifFalse", "ifFalse", doubleArray, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(ifTrueAttr, "ifTrue", "ifTrue", doubleArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(outputAttr, "output", "output", doubleArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectDoubleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> ifFalse = inputDoubleDataArrayValue(dataBlock, ifFalseAttr);
	std::vector<double> ifTrue = inputDoubleDataArrayValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputDoubleDataArrayValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------