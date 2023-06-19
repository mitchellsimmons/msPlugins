#include "normalizeDoubleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NormalizeDoubleArray::NormalizeDoubleArray() {}
NormalizeDoubleArray::~NormalizeDoubleArray() {}

// ------ Attr ------
MObject NormalizeDoubleArray::inputAttr;
MObject NormalizeDoubleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NormalizeDoubleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NormalizeDoubleArray::initialize()
{
	std::vector<double> array;

	createDoubleDataArrayAttribute(inputAttr, "input", "input", array, kDefaultPreset | kKeyable);
	createDoubleDataArrayAttribute(outputAttr, "output", "output", array, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NormalizeDoubleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<double> values = inputDoubleDataArrayValue(dataBlock, inputAttr);

	double sum = 0;
	for (const double& input : values)
		sum += input;
	
	// Since inputs can be both positive and negative and to ensure values extremely close to zero are not normalized
	if (!MRS::isEqual(sum, 0.0))
	{
		for (double& input : values)
			input /= sum;
	}

	outputDoubleDataArrayValue(dataBlock, outputAttr, values);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------