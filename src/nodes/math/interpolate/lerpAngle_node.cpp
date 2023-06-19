#include "lerpAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LerpAngle::LerpAngle() {}
LerpAngle::~LerpAngle() {}

// ------ Attr ------
MObject LerpAngle::input1Attr;
MObject LerpAngle::input2Attr;
MObject LerpAngle::tAttr;
MObject LerpAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType LerpAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus LerpAngle::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(tAttr, "t", "t", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tAttr, 0.0, 1.0);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(tAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(tAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus LerpAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputAngleValue(dataBlock, input2Attr).asRadians();
	double t = inputDoubleValue(dataBlock, tAttr);

	outputAngleValue(dataBlock, outputAttr, input1 + (input2 - input1) * t);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------