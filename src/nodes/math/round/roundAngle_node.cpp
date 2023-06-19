#include "roundAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RoundAngle::RoundAngle() {}
RoundAngle::~RoundAngle() {}

// ------ Attr ------
MObject RoundAngle::inputAttr;
MObject RoundAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RoundAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RoundAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RoundAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::round(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------