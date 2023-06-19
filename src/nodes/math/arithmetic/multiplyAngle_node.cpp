#include "multiplyAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyAngle::MultiplyAngle() {}
MultiplyAngle::~MultiplyAngle() {}

// ------ Attr ------
MObject MultiplyAngle::input1Attr;
MObject MultiplyAngle::input2Attr;
MObject MultiplyAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyAngle::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputAngleValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------