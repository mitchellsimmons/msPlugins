#include "clamp_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Clamp::Clamp() {}
Clamp::~Clamp() {}

// ------ Attr ------
MObject Clamp::inputAttr;
MObject Clamp::minAttr;
MObject Clamp::maxAttr;
MObject Clamp::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Clamp::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Clamp::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(minAttr, "min", "min", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(maxAttr, "max", "max", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(minAttr);
	addAttribute(maxAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(minAttr, outputAttr);
	attributeAffects(maxAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Clamp::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);
	double min = inputDoubleValue(dataBlock, minAttr);
	double max = inputDoubleValue(dataBlock, maxAttr);

	outputDoubleValue(dataBlock, outputAttr, MRS::clamp(input, min, max));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------