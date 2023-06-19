#include "extractQuaternionFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractQuaternionFromMatrix::ExtractQuaternionFromMatrix() {}
ExtractQuaternionFromMatrix::~ExtractQuaternionFromMatrix() {}

// ------ Attr ------
MObject ExtractQuaternionFromMatrix::inputAttr;
MObject ExtractQuaternionFromMatrix::outputAttr;
MObject ExtractQuaternionFromMatrix::outputXAttr;
MObject ExtractQuaternionFromMatrix::outputYAttr;
MObject ExtractQuaternionFromMatrix::outputZAttr;
MObject ExtractQuaternionFromMatrix::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractQuaternionFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractQuaternionFromMatrix::initialize()
{
	MMatrix input;
	MQuaternion output;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractQuaternionFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, MRS::extractQuaternionRotation(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------