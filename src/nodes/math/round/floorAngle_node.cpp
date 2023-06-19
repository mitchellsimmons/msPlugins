#include "floorAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FloorAngle::FloorAngle() {}
FloorAngle::~FloorAngle() {}

// ------ Attr ------
MObject FloorAngle::inputAttr;
MObject FloorAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType FloorAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus FloorAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus FloorAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::floor(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------