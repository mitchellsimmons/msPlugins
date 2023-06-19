#include "weightedAverageAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverageAngle::WeightedAverageAngle() {}
WeightedAverageAngle::~WeightedAverageAngle() {}

// ------ Attr ------
MObject WeightedAverageAngle::inputAttr;
MObject WeightedAverageAngle::weightAttr;
MObject WeightedAverageAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverageAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverageAngle::initialize()
{
	std::vector<MAngle> inputs;
	std::vector<double> weights;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weights, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverageAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> inputs = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, inputAttr);
	std::vector<double> weights = inputDoubleDataArrayValue(dataBlock, weightAttr);
	size_t count = inputs.size();
	weights.resize(count, 1.0);

	double inputSum = 0.0;
	double weightSum = 0.0;

	for (unsigned int i = 0; i < count; ++i)
	{
		inputSum += inputs[i].asRadians() * weights[i];
		weightSum += weights[i];
	}

	outputAngleValue(dataBlock, outputAttr, MRS::isEqual(weightSum, 0.0) ? 0.0 : inputSum / weightSum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------