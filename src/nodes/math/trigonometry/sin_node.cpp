#include "sin_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Sin::Sin() {}
Sin::~Sin() {}

// ------ Attr ------
MObject Sin::inputAttr;
MObject Sin::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Sin::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Sin::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Sin::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputDoubleValue(dataBlock, outputAttr, std::sin(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------