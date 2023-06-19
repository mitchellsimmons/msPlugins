#include "absoluteAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AbsoluteAngle::AbsoluteAngle() {}
AbsoluteAngle::~AbsoluteAngle() {}

// ------ Attr ------
MObject AbsoluteAngle::inputAttr;
MObject AbsoluteAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AbsoluteAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AbsoluteAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AbsoluteAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, std::abs(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------