#include "minAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MinAngle::MinAngle() {}
MinAngle::~MinAngle() {}

// ------ Attr ------
MObject MinAngle::input1Attr;
MObject MinAngle::input2Attr;
MObject MinAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MinAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MinAngle::initialize()
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

MStatus MinAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputAngleValue(dataBlock, input2Attr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::min(input1, input2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------