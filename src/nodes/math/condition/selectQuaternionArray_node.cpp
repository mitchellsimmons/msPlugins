#include "selectQuaternionArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectQuaternionArray::SelectQuaternionArray() {}
SelectQuaternionArray::~SelectQuaternionArray() {}

// ------ Attr ------
MObject SelectQuaternionArray::ifFalseAttr;
MObject SelectQuaternionArray::ifTrueAttr;
MObject SelectQuaternionArray::conditionAttr;
MObject SelectQuaternionArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectQuaternionArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectQuaternionArray::initialize()
{
	std::vector<MQuaternion> quaternionArray;

	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(ifFalseAttr, "ifFalse", "ifFalse", quaternionArray, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(ifTrueAttr, "ifTrue", "ifTrue", quaternionArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(outputAttr, "output", "output", quaternionArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectQuaternionArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MQuaternion> ifFalse = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, ifFalseAttr);
	std::vector<MQuaternion> ifTrue = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------