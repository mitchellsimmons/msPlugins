#include "weightedAverage_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverage::WeightedAverage() {}
WeightedAverage::~WeightedAverage() {}

// ------ Attr ------
MObject WeightedAverage::inputAttr;
MObject WeightedAverage::weightAttr;
MObject WeightedAverage::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverage::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverage::initialize()
{
	std::vector<double> inputs;
	std::vector<double> weights;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weights, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverage::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> inputs = inputDoubleDataArrayValue(dataBlock, inputAttr);
	std::vector<double> weights = inputDoubleDataArrayValue(dataBlock, weightAttr);
	size_t count = inputs.size();
	weights.resize(count, 1.0);

	double inputSum = 0.0;
	double weightSum = 0.0;

	for (unsigned int i = 0; i < count; ++i)
	{
		inputSum += inputs[i] * weights[i];
		weightSum += weights[i];
	}

	outputDoubleValue(dataBlock, outputAttr, MRS::isEqual(weightSum, 0.0) ? 0.0 : inputSum / weightSum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------