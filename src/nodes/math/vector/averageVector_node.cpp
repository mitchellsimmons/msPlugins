#include "averageVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageVector::AverageVector() {}
AverageVector::~AverageVector() {}

// ------ Attr ------
MObject AverageVector::inputAttr;
MObject AverageVector::outputXAttr;
MObject AverageVector::outputYAttr;
MObject AverageVector::outputZAttr;
MObject AverageVector::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageVector::initialize()
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

MStatus AverageVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MVector> inputs = inputVectorDataArrayValue(dataBlock, inputAttr);

	outputVectorValue(dataBlock, outputAttr, MRS::averageVector(inputs));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------