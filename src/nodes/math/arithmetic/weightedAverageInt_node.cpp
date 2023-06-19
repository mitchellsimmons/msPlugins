#include "weightedAverageInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverageInt::WeightedAverageInt() {}
WeightedAverageInt::~WeightedAverageInt() {}

// ------ Attr ------
MObject WeightedAverageInt::inputAttr;
MObject WeightedAverageInt::weightAttr;
MObject WeightedAverageInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverageInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverageInt::initialize()
{
	std::vector<int> inputs;
	std::vector<double> weights;

	createIntDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weights, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverageInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputs = inputIntDataArrayValue(dataBlock, inputAttr);
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