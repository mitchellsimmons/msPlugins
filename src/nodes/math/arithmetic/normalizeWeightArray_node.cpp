#include "normalizeWeightArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NormalizeWeightArray::NormalizeWeightArray() {}
NormalizeWeightArray::~NormalizeWeightArray() {}

// ------ Attr ------
MObject NormalizeWeightArray::inputAttr;
MObject NormalizeWeightArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NormalizeWeightArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NormalizeWeightArray::initialize()
{
	std::vector<double> array;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", array, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(outputAttr, "output", "output", array, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NormalizeWeightArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> values = inputDoubleDataArrayValue(dataBlock, inputAttr);

	for (double& input : values)
		input = std::max(0.0, std::min(1.0, input));

	double sum = 0.0;
	for (const double& input : values)
		sum += input;

	if (sum < 1.0)
		outputDoubleDataArrayValue(dataBlock, outputAttr, values);
	else
	{
		// To ensure values extremely close to zero are not normalized
		if (!MRS::isEqual(sum, 0.0))
		{
		for (double& input : values)
			input /= sum;
		}

		outputDoubleDataArrayValue(dataBlock, outputAttr, values);
	}

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------