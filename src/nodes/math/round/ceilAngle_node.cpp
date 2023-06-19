#include "ceilAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CeilAngle::CeilAngle() {}
CeilAngle::~CeilAngle() {}

// ------ Attr ------
MObject CeilAngle::inputAttr;
MObject CeilAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType CeilAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus CeilAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus CeilAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::ceil(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------