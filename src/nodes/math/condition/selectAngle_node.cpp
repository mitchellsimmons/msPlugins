#include "selectAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectAngle::SelectAngle() {}
SelectAngle::~SelectAngle() {}

// ------ Attr ------
MObject SelectAngle::ifFalseAttr;
MObject SelectAngle::ifTrueAttr;
MObject SelectAngle::conditionAttr;
MObject SelectAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectAngle::initialize()
{
	createAngleAttribute(ifFalseAttr, "ifFalse", "ifFalse", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(ifTrueAttr, "ifTrue", "ifTrue", 0.0, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double ifTrue = inputAngleValue(dataBlock, ifTrueAttr).asRadians();
	double ifFalse = inputAngleValue(dataBlock, ifFalseAttr).asRadians();
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputAngleValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------