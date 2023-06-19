#include "sumVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SumVector::SumVector() {}
SumVector::~SumVector() {}

// ------ Attr ------
MObject SumVector::inputAttr;
MObject SumVector::outputAttr;
MObject SumVector::outputXAttr;
MObject SumVector::outputYAttr;
MObject SumVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SumVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SumVector::initialize()
{
	std::vector<MVector> input;
	MVector output = MVector::zero;

	createVectorDataArrayAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SumVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MVector> inputs = inputVectorDataArrayValue(dataBlock, inputAttr);

	MVector sum = MVector::zero;
	for (const MVector& input : inputs)
		sum += input;

	outputVectorValue(dataBlock, outputAttr, sum);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------