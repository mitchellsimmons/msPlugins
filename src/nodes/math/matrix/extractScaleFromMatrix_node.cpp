#include "extractScaleFromMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractScaleFromMatrix::ExtractScaleFromMatrix() : MPxNode(), NodeHelper() {}
ExtractScaleFromMatrix::~ExtractScaleFromMatrix() {}

// ------ Attr ------
MObject ExtractScaleFromMatrix::inputAttr;
MObject ExtractScaleFromMatrix::outputAttr;
MObject ExtractScaleFromMatrix::outputXAttr;
MObject ExtractScaleFromMatrix::outputYAttr;
MObject ExtractScaleFromMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractScaleFromMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractScaleFromMatrix::initialize()
{
	MMatrix input;
	MVector output;

	createMatrixAttribute(inputAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractScaleFromMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputVectorValue(dataBlock, outputAttr, MRS::extractScale(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------