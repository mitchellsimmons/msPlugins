#include "multiplyMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyMatrixArray::MultiplyMatrixArray() : MPxNode(), NodeHelper() {}
MultiplyMatrixArray::~MultiplyMatrixArray() {}

// ------ Attr ------
MObject MultiplyMatrixArray::inputAttr;
MObject MultiplyMatrixArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyMatrixArray::initialize()
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

MStatus MultiplyMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MMatrix> inputs = inputMatrixDataArrayValue(dataBlock, inputAttr);
	size_t count = inputs.size();

	if (count == 0)
		outputMatrixValue(dataBlock, outputAttr, MMatrix::identity);
	else
	{
		MMatrix output = inputs[0];
		for (unsigned int i = 1; i < count; ++i)
			output *= inputs[i];

		outputMatrixValue(dataBlock, outputAttr, output);
	}

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------