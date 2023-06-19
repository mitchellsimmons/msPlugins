#include "cos_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Cos::Cos() {}
Cos::~Cos() {}

// ------ Attr ------
MObject Cos::inputAttr;
MObject Cos::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Cos::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Cos::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 1.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Cos::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputDoubleValue(dataBlock, outputAttr, std::cos(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------