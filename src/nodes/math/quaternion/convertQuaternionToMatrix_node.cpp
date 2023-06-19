#include "convertQuaternionToMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ConvertQuaternionToMatrix::ConvertQuaternionToMatrix() {}
ConvertQuaternionToMatrix::~ConvertQuaternionToMatrix() {}

// ------ Attr ------
MObject ConvertQuaternionToMatrix::inputAttr;
MObject ConvertQuaternionToMatrix::inputXAttr;
MObject ConvertQuaternionToMatrix::inputYAttr;
MObject ConvertQuaternionToMatrix::inputZAttr;
MObject ConvertQuaternionToMatrix::inputWAttr;
MObject ConvertQuaternionToMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ConvertQuaternionToMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ConvertQuaternionToMatrix::initialize()
{
	MQuaternion input = MQuaternion::identity;
	MMatrix output = MMatrix::identity;

	createQuaternionAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, inputWAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ConvertQuaternionToMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MQuaternion input = inputQuaternionValue(dataBlock, inputAttr, inputXAttr, inputYAttr, inputZAttr, inputWAttr);

	outputMatrixValue(dataBlock, outputAttr, input.asMatrix());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------