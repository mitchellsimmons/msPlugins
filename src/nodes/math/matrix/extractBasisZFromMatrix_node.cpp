#include "extractBasisZFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractBasisZFromMatrix::ExtractBasisZFromMatrix() : MPxNode(), NodeHelper() {}
ExtractBasisZFromMatrix::~ExtractBasisZFromMatrix() {}

// ------ Attr ------
MObject ExtractBasisZFromMatrix::inputAttr;
MObject ExtractBasisZFromMatrix::normalizeAttr;
MObject ExtractBasisZFromMatrix::outputAttr;
MObject ExtractBasisZFromMatrix::outputXAttr;
MObject ExtractBasisZFromMatrix::outputYAttr;
MObject ExtractBasisZFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractBasisZFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractBasisZFromMatrix::initialize()
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

MStatus ExtractBasisZFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);
	bool normalize = inputBoolValue(dataBlock, normalizeAttr);

	MVector output{ input[2][0], input[2][1], input[2][2] };
	if (normalize)
		output.normalize();

	outputVectorValue(dataBlock, outputAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------