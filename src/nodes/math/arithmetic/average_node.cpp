#include "average_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Average::Average() {}
Average::~Average() {}

// ------ Attr ------
MObject Average::inputAttr;
MObject Average::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Average::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Average::initialize()
{
	std::vector<double> inputs;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Average::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> inputs = inputDoubleDataArrayValue(dataBlock, inputAttr);

	size_t count = inputs.size();
	double sum = 0.0;
	for (const double& input : inputs)
		sum += input;

	outputDoubleValue(dataBlock, outputAttr, count == 0 ? 0.0 : sum / inputs.size());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------