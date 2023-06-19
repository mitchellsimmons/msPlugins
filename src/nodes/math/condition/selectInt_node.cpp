#include "selectInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectInt::SelectInt() {}
SelectInt::~SelectInt() {}

// ------ Attr ------
MObject SelectInt::ifFalseAttr;
MObject SelectInt::ifTrueAttr;
MObject SelectInt::conditionAttr;
MObject SelectInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectInt::initialize()
{
	createIntAttribute(ifFalseAttr, "ifFalse", "ifFalse", 0, kDefaultPreset | kKeyable);
	createIntAttribute(ifTrueAttr, "ifTrue", "ifTrue", 0, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int ifFalse = inputIntValue(dataBlock, ifFalseAttr);
	int ifTrue = inputIntValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputIntValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------