#include "tan_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Tan::Tan() {}
Tan::~Tan() {}

// ------ Attr ------
MObject Tan::inputAttr;
MObject Tan::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Tan::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Tan::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Tan::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();

	outputDoubleValue(dataBlock, outputAttr, std::tan(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------