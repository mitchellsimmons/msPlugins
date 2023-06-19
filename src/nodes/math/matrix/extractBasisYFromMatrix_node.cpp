#include "extractBasisYFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractBasisYFromMatrix::ExtractBasisYFromMatrix() : MPxNode(), NodeHelper() {}
ExtractBasisYFromMatrix::~ExtractBasisYFromMatrix() {}

// ------ Attr ------
MObject ExtractBasisYFromMatrix::inputAttr;
MObject ExtractBasisYFromMatrix::normalizeAttr;
MObject ExtractBasisYFromMatrix::outputAttr;
MObject ExtractBasisYFromMatrix::outputXAttr;
MObject ExtractBasisYFromMatrix::outputYAttr;
MObject ExtractBasisYFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractBasisYFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractBasisYFromMatrix::initialize()
{
	MMatrix input;
	MVector output;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createBoolAttribute(normalizeAttr, "normalize", "normalize", true, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(normalizeAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(normalizeAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractBasisYFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);
	bool normalize = inputBoolValue(dataBlock, normalizeAttr);

	MVector output{ input[1][0], input[1][1], input[1][2] };
	if (normalize)
		output.normalize();

	outputVectorValue(dataBlock, outputAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------