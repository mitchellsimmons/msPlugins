#include "round_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Round::Round() {}
Round::~Round() {}

// ------ Attr ------
MObject Round::inputAttr;
MObject Round::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Round::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Round::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Round::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, std::round(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------