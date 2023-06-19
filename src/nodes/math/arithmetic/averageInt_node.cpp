#include "averageInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageInt::AverageInt() {}
AverageInt::~AverageInt() {}

// ------ Attr ------
MObject AverageInt::inputAttr;
MObject AverageInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageInt::initialize()
{
	std::vector<int> inputs;

	createIntDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AverageInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputs = inputIntDataArrayValue(dataBlock, inputAttr);

	size_t count = inputs.size();
	int sum = 0;
	for (const int& input : inputs)
		sum += input;

	outputDoubleValue(dataBlock, outputAttr, count == 0 ? 0.0 : (double)sum / inputs.size());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------