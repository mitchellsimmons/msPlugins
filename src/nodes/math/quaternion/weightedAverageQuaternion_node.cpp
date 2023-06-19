#include "weightedAverageQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverageQuaternion::WeightedAverageQuaternion() {}
WeightedAverageQuaternion::~WeightedAverageQuaternion() {}

// ------ Attr ------
MObject WeightedAverageQuaternion::inputAttr;
MObject WeightedAverageQuaternion::weightAttr;
MObject WeightedAverageQuaternion::outputAttr;
MObject WeightedAverageQuaternion::outputXAttr;
MObject WeightedAverageQuaternion::outputYAttr;
MObject WeightedAverageQuaternion::outputZAttr;
MObject WeightedAverageQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverageQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverageQuaternion::initialize()
{
	std::vector<MQuaternion> inputs;
	std::vector<double> weights;
	MQuaternion output;

	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weights, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverageQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MQuaternion> inputs = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, inputAttr);
	std::vector<double> weights = inputDoubleDataArrayValue(dataBlock, weightAttr);
	size_t count = inputs.size();
	weights.resize(count, 1.0);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, MRS::averageWeightedQuaternion(inputs, weights));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------