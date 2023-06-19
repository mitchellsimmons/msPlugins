#include "subtractAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SubtractAngle::SubtractAngle() {}
SubtractAngle::~SubtractAngle() {}

// ------ Attr ------
MObject SubtractAngle::input1Attr;
MObject SubtractAngle::input2Attr;
MObject SubtractAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SubtractAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SubtractAngle::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SubtractAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputAngleValue(dataBlock, input2Attr).asRadians();

	outputAngleValue(dataBlock, outputAttr, input1 - input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------