#include "atan_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Atan::Atan() {}
Atan::~Atan() {}

// ------ Attr ------
MObject Atan::inputAttr;
MObject Atan::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Atan::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Atan::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Atan::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputAngleValue(dataBlock, outputAttr, std::atan(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------