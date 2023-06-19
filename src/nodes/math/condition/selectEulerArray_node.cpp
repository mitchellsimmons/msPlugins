#include "selectEulerArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectEulerArray::SelectEulerArray() {}
SelectEulerArray::~SelectEulerArray() {}

// ------ Attr ------
MObject SelectEulerArray::ifFalseAttr;
MObject SelectEulerArray::ifTrueAttr;
MObject SelectEulerArray::conditionAttr;
MObject SelectEulerArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectEulerArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectEulerArray::initialize()
{
	std::vector<MEulerRotation> eulerArray;

	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(ifFalseAttr, "ifFalse", "ifFalse", eulerArray, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(ifTrueAttr, "ifTrue", "ifTrue", eulerArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputAttr, "output", "output", eulerArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectEulerArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MEulerRotation> ifFalse = inputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, ifFalseAttr);
	std::vector<MEulerRotation> ifTrue = inputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------