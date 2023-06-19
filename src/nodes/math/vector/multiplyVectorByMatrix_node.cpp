#include "multiplyVectorByMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyVectorByMatrix::MultiplyVectorByMatrix() {}
MultiplyVectorByMatrix::~MultiplyVectorByMatrix() {}

// ------ Attr ------
MObject MultiplyVectorByMatrix::input1Attr;
MObject MultiplyVectorByMatrix::input1XAttr;
MObject MultiplyVectorByMatrix::input1YAttr;
MObject MultiplyVectorByMatrix::input1ZAttr;
MObject MultiplyVectorByMatrix::input2Attr;
MObject MultiplyVectorByMatrix::outputAttr;
MObject MultiplyVectorByMatrix::outputXAttr;
MObject MultiplyVectorByMatrix::outputYAttr;
MObject MultiplyVectorByMatrix::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyVectorByMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyVectorByMatrix::initialize()
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

MStatus MultiplyVectorByMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MMatrix input2 = inputMatrixValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------