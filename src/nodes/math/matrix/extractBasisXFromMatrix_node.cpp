#include "extractBasisXFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractBasisXFromMatrix::ExtractBasisXFromMatrix() : MPxNode(), NodeHelper() {}
ExtractBasisXFromMatrix::~ExtractBasisXFromMatrix() {}

// ------ Attr ------
MObject ExtractBasisXFromMatrix::inputAttr;
MObject ExtractBasisXFromMatrix::normalizeAttr;
MObject ExtractBasisXFromMatrix::outputAttr;
MObject ExtractBasisXFromMatrix::outputXAttr;
MObject ExtractBasisXFromMatrix::outputYAttr;
MObject ExtractBasisXFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractBasisXFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractBasisXFromMatrix::initialize()
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

MStatus ExtractBasisXFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);
	bool normalize = inputBoolValue(dataBlock, normalizeAttr);

	MVector output{ input[0][0], input[0][1], input[0][2] };
	if (normalize)
		output.normalize();

	outputVectorValue(dataBlock, outputAttr, output);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------