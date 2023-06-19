#include "sumInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SumInt::SumInt() {}
SumInt::~SumInt() {}

// ------ Attr ------
MObject SumInt::inputAttr;
MObject SumInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SumInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SumInt::initialize()
{
	std::vector<int> inputs;

	createIntDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SumInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputs = inputIntDataArrayValue(dataBlock, inputAttr);

	int sum = 0;
	for (const int& input : inputs)
		sum += input;

	outputIntValue(dataBlock, outputAttr, sum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------