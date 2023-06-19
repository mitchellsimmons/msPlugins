#include "absolute_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Absolute::Absolute() {}
Absolute::~Absolute() {}

// ------ Attr ------
MObject Absolute::inputAttr;
MObject Absolute::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Absolute::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Absolute::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Absolute::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);
	
	outputDoubleValue(dataBlock, outputAttr, std::abs(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------