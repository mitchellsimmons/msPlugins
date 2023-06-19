#include "selectMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectMatrixArray::SelectMatrixArray() {}
SelectMatrixArray::~SelectMatrixArray() {}

// ------ Attr ------
MObject SelectMatrixArray::ifFalseAttr;
MObject SelectMatrixArray::ifTrueAttr;
MObject SelectMatrixArray::conditionAttr;
MObject SelectMatrixArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectMatrixArray::initialize()
{
	std::vector<MMatrix> matrixArray;

	createMatrixDataArrayAttribute(ifFalseAttr, "ifFalse", "ifFalse", matrixArray, kDefaultPreset | kKeyable);
	createMatrixDataArrayAttribute(ifTrueAttr, "ifTrue", "ifTrue", matrixArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createMatrixDataArrayAttribute(outputAttr, "output", "output", matrixArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MMatrix> ifFalse = inputMatrixDataArrayValue(dataBlock, ifFalseAttr);
	std::vector<MMatrix> ifTrue = inputMatrixDataArrayValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputMatrixDataArrayValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------