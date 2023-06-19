#include "negateAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NegateAngle::NegateAngle() {}
NegateAngle::~NegateAngle() {}

// ------ Attr ------
MObject NegateAngle::inputAttr;
MObject NegateAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NegateAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NegateAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NegateAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputAngleValue(dataBlock, outputAttr, -input);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------