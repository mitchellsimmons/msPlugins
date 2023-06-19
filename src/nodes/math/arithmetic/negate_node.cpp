#include "negate_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Negate::Negate() {}
Negate::~Negate() {}

// ------ Attr ------
MObject Negate::inputAttr;
MObject Negate::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Negate::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Negate::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Negate::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, -input);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------