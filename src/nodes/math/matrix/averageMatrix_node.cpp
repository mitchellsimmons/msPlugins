#include "averageMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageMatrix::AverageMatrix() {}
AverageMatrix::~AverageMatrix() {}

// ------ Attr ------
MObject AverageMatrix::inputAttr;
MObject AverageMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageMatrix::initialize()
{
	std::vector<MMatrix> input;
	MMatrix output;

	createMatrixDataArrayAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AverageMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MMatrix> inputs = inputMatrixDataArrayValue(dataBlock, inputAttr);
	size_t count = inputs.size();

	MMatrix average = MMatrix::identity;
	if (count)
	{
		m_translation.resize(count);
		m_rotation.resize(count);
		m_scale.resize(count);

		for (unsigned int i = 0; i < count; ++i)
			MRS::decomposeMatrix(inputs[i], m_translation[i], m_rotation[i], m_scale[i]);

		MVector translationAverage = MRS::averageVector(m_translation);
		MQuaternion rotationAverage = MRS::averageQuaternion(m_rotation);
		MVector scaleAverage = MRS::averageVector(m_scale);

		average = MRS::composeMatrix(translationAverage, rotationAverage, scaleAverage);
	}

	outputMatrixValue(dataBlock, outputAttr, average);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------