#include "selectMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectMatrix::SelectMatrix() {}
SelectMatrix::~SelectMatrix() {}

// ------ Attr ------
MObject SelectMatrix::ifFalseAttr;
MObject SelectMatrix::ifTrueAttr;
MObject SelectMatrix::conditionAttr;
MObject SelectMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(ifFalseAttr, "ifFalse", "ifFalse", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(ifTrueAttr, "ifTrue", "ifTrue", matrix, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix ifFalse = inputMatrixValue(dataBlock, ifFalseAttr);
	MMatrix ifTrue = inputMatrixValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputMatrixValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------