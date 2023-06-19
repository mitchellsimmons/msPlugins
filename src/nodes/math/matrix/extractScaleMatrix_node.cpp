#include "extractScaleMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractScaleMatrix::ExtractScaleMatrix() : MPxNode(), NodeHelper() {}
ExtractScaleMatrix::~ExtractScaleMatrix() {}

// ------ Attr ------
MObject ExtractScaleMatrix::inputAttr;
MObject ExtractScaleMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractScaleMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractScaleMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractScaleMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputMatrixValue(dataBlock, outputAttr, MRS::extractScaleMatrix(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------