#include "maxElementInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MaxElementInt::MaxElementInt() {}
MaxElementInt::~MaxElementInt() {}

// ------ Attr ------
MObject MaxElementInt::inputAttr;
MObject MaxElementInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MaxElementInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MaxElementInt::initialize()
{
	std::vector<int> inputs;

	createIntDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MaxElementInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputs = inputIntDataArrayValue(dataBlock, inputAttr);

	auto max = std::max_element(inputs.cbegin(), inputs.cend());

	outputIntValue(dataBlock, outputAttr, max != inputs.cend() ? *max : 0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------