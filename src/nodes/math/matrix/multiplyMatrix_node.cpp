#include "multiplyMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyMatrix::MultiplyMatrix() : MPxNode(), NodeHelper() {}
MultiplyMatrix::~MultiplyMatrix() {}

// ------ Attr ------
MObject MultiplyMatrix::input1Attr;
MObject MultiplyMatrix::input2Attr;
MObject MultiplyMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(input1Attr, "input1", "input1", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(input2Attr, "input2", "input2", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input1 = inputMatrixValue(dataBlock, input1Attr);
	MMatrix input2 = inputMatrixValue(dataBlock, input2Attr);

	outputMatrixValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------