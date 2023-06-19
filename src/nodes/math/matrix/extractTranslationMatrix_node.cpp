#include "extractTranslationMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractTranslationMatrix::ExtractTranslationMatrix() : MPxNode(), NodeHelper() {}
ExtractTranslationMatrix::~ExtractTranslationMatrix() {}

// ------ Attr ------
MObject ExtractTranslationMatrix::inputAttr;
MObject ExtractTranslationMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractTranslationMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractTranslationMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractTranslationMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputMatrixValue(dataBlock, outputAttr, MRS::extractTranslationMatrix(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------