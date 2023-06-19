#include "multiplyPointByMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyPointByMatrix::MultiplyPointByMatrix() {}
MultiplyPointByMatrix::~MultiplyPointByMatrix() {}

// ------ Attr ------
MObject MultiplyPointByMatrix::input1Attr;
MObject MultiplyPointByMatrix::input1XAttr;
MObject MultiplyPointByMatrix::input1YAttr;
MObject MultiplyPointByMatrix::input1ZAttr;
MObject MultiplyPointByMatrix::input2Attr;
MObject MultiplyPointByMatrix::outputAttr;
MObject MultiplyPointByMatrix::outputXAttr;
MObject MultiplyPointByMatrix::outputYAttr;
MObject MultiplyPointByMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyPointByMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyPointByMatrix::initialize()
{
	MVector vector = MVector::zero;
	MMatrix matrix = MMatrix::identity;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createMatrixAttribute(input2Attr, "input2", "input2", matrix, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyPointByMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MPoint input1 = inputVectorValue(dataBlock, input1Attr);
	MMatrix input2 = inputMatrixValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------