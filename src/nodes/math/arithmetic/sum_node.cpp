#include "sum_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Sum::Sum() {}
Sum::~Sum() {}

// ------ Attr ------
MObject Sum::inputAttr;
MObject Sum::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Sum::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Sum::initialize()
{
	std::vector<double> inputs;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Sum::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> inputs = inputDoubleDataArrayValue(dataBlock, inputAttr);

	double sum = 0.0;
	for (const double& input : inputs)
		sum += input;

	outputDoubleValue(dataBlock, outputAttr, sum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------