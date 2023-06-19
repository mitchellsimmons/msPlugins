#include "extractTranslationFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractTranslationFromMatrix::ExtractTranslationFromMatrix() : MPxNode(), NodeHelper() {}
ExtractTranslationFromMatrix::~ExtractTranslationFromMatrix() {}

// ------ Attr ------
MObject ExtractTranslationFromMatrix::inputAttr;
MObject ExtractTranslationFromMatrix::outputAttr;
MObject ExtractTranslationFromMatrix::outputXAttr;
MObject ExtractTranslationFromMatrix::outputYAttr;
MObject ExtractTranslationFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractTranslationFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractTranslationFromMatrix::initialize()
{
	MMatrix input;
	MVector output;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, 
		"output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractTranslationFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputVectorValue(dataBlock, outputAttr, MRS::extractTranslation(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------