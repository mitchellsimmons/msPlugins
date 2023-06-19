#include "minElementInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MinElementInt::MinElementInt() {}
MinElementInt::~MinElementInt() {}

// ------ Attr ------
MObject MinElementInt::inputAttr;
MObject MinElementInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MinElementInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MinElementInt::initialize()
{
	std::vector<int> inputs;

	createIntDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MinElementInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputs = inputIntDataArrayValue(dataBlock, inputAttr);

	auto min = std::min_element(inputs.cbegin(), inputs.cend());

	outputIntValue(dataBlock, outputAttr, min != inputs.cend() ? *min : 0);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------