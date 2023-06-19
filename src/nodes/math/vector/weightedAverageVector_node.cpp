#include "weightedAverageVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverageVector::WeightedAverageVector() {}
WeightedAverageVector::~WeightedAverageVector() {}

// ------ Attr ------
MObject WeightedAverageVector::inputAttr;
MObject WeightedAverageVector::weightAttr;
MObject WeightedAverageVector::outputXAttr;
MObject WeightedAverageVector::outputYAttr;
MObject WeightedAverageVector::outputZAttr;
MObject WeightedAverageVector::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverageVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverageVector::initialize()
{
	std::vector<MVector> inputs;
	std::vector<double> weights;
	MVector output = MVector::zero;

	createVectorDataArrayAttribute(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weights, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverageVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MVector> inputs = inputVectorDataArrayValue(dataBlock, inputAttr);
	std::vector<double> weights = inputDoubleDataArrayValue(dataBlock, weightAttr);
	size_t count = inputs.size();
	weights.resize(count, 1.0);

	outputVectorValue(dataBlock, outputAttr, MRS::averageWeightedVector(inputs, weights));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------