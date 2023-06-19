#include "addAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AddAngle::AddAngle() {}
AddAngle::~AddAngle() {}

// ------ Attr ------
MObject AddAngle::input1Attr;
MObject AddAngle::input2Attr;
MObject AddAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AddAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AddAngle::initialize()
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

MStatus AddAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputAngleValue(dataBlock, input2Attr).asRadians();

	outputAngleValue(dataBlock, outputAttr, input1 + input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------