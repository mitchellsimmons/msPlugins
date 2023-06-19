#include "maxAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MaxAngle::MaxAngle() {}
MaxAngle::~MaxAngle() {}

// ------ Attr ------
MObject MaxAngle::input1Attr;
MObject MaxAngle::input2Attr;
MObject MaxAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MaxAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MaxAngle::initialize()
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

MStatus MaxAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputAngleValue(dataBlock, input2Attr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::max(input1, input2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------