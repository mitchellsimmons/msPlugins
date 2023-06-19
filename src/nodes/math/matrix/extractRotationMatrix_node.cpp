#include "extractRotationMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ExtractRotationMatrix::ExtractRotationMatrix() : MPxNode(), NodeHelper() {}
ExtractRotationMatrix::~ExtractRotationMatrix() {}

// ------ Attr ------
MObject ExtractRotationMatrix::inputAttr;
MObject ExtractRotationMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ExtractRotationMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ExtractRotationMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ExtractRotationMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input = inputMatrixValue(dataBlock, inputAttr);

	outputMatrixValue(dataBlock, outputAttr, MRS::extractRotationMatrix(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------