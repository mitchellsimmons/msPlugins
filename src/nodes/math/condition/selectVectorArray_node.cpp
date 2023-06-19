#include "selectVectorArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectVectorArray::SelectVectorArray() {}
SelectVectorArray::~SelectVectorArray() {}

// ------ Attr ------
MObject SelectVectorArray::ifFalseAttr;
MObject SelectVectorArray::ifTrueAttr;
MObject SelectVectorArray::conditionAttr;
MObject SelectVectorArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectVectorArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectVectorArray::initialize()
{
	std::vector<MVector> vectorArray;

	createVectorDataArrayAttribute(ifFalseAttr, "ifFalse", "ifFalse", vectorArray, kDefaultPreset | kKeyable);
	createVectorDataArrayAttribute(ifTrueAttr, "ifTrue", "ifTrue", vectorArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createVectorDataArrayAttribute(outputAttr, "output", "output", vectorArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectVectorArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MVector> ifFalse = inputVectorDataArrayValue(dataBlock, ifFalseAttr);
	std::vector<MVector> ifTrue = inputVectorDataArrayValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputVectorDataArrayValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------