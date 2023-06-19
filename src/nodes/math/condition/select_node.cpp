#include "select_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Select::Select() {}
Select::~Select() {}

// ------ Attr ------
MObject Select::ifFalseAttr;
MObject Select::ifTrueAttr;
MObject Select::conditionAttr;
MObject Select::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Select::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Select::initialize()
{
	createDoubleAttribute(ifFalseAttr, "ifFalse", "ifFalse", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(ifTrueAttr, "ifTrue", "ifTrue", 0.0, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Select::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double ifFalse = inputDoubleValue(dataBlock, ifFalseAttr);
	double ifTrue = inputDoubleValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputDoubleValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------