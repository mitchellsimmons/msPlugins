#include "clampAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ClampAngle::ClampAngle() {}
ClampAngle::~ClampAngle() {}

// ------ Attr ------
MObject ClampAngle::inputAttr;
MObject ClampAngle::minAttr;
MObject ClampAngle::maxAttr;
MObject ClampAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ClampAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ClampAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(minAttr, "min", "min", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(maxAttr, "max", "max", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(minAttr);
	addAttribute(maxAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(minAttr, outputAttr);
	attributeAffects(maxAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ClampAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();
	double min = inputAngleValue(dataBlock, minAttr).asRadians();
	double max = inputAngleValue(dataBlock, maxAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, MRS::clamp(input, min, max));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------