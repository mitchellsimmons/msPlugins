#include "weightedAverageMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

WeightedAverageMatrix::WeightedAverageMatrix() {}
WeightedAverageMatrix::~WeightedAverageMatrix() {}

// ------ Attr ------
MObject WeightedAverageMatrix::inputAttr;
MObject WeightedAverageMatrix::weightAttr;
MObject WeightedAverageMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType WeightedAverageMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus WeightedAverageMatrix::initialize()
{
	std::vector<MMatrix> input;
	std::vector<double> weight;
	MMatrix output;

	createMatrixDataArrayAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(weightAttr, "weight", "weight", weight, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(weightAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(weightAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus WeightedAverageMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MMatrix> inputs = inputMatrixDataArrayValue(dataBlock, inputAttr);
	std::vector<double> weights = inputDoubleDataArrayValue(dataBlock, weightAttr);
	size_t count = inputs.size();
	weights.resize(count, 1.0);

	MMatrix average = MMatrix::identity;
	if (count)
	{
		m_translation.resize(count);
		m_rotation.resize(count);
		m_scale.resize(count);

		for (unsigned int i = 0; i < count; ++i)
			MRS::decomposeMatrix(inputs[i], m_translation[i], m_rotation[i], m_scale[i]);

		MVector translationAverage = MRS::averageWeightedVector(m_translation, weights);
		MQuaternion rotationAverage = MRS::averageWeightedQuaternion(m_rotation, weights);
		MVector scaleAverage = MRS::averageWeightedVector(m_scale, weights);

		average = MRS::composeMatrix(translationAverage, rotationAverage, scaleAverage);
	}

	outputMatrixValue(dataBlock, outputAttr, average);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------