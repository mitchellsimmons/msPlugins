#include "ceil_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Ceil::Ceil() {}
Ceil::~Ceil() {}

// ------ Attr ------
MObject Ceil::inputAttr;
MObject Ceil::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Ceil::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Ceil::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Ceil::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, std::ceil(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------