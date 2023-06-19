#include "selectAngleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectAngleArray::SelectAngleArray() {}
SelectAngleArray::~SelectAngleArray() {}

// ------ Attr ------
MObject SelectAngleArray::ifFalseAttr;
MObject SelectAngleArray::ifTrueAttr;
MObject SelectAngleArray::conditionAttr;
MObject SelectAngleArray::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectAngleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectAngleArray::initialize()
{
	std::vector<MAngle> angleArray;

	createPluginDataArrayAttribute<AngleArrayData, MAngle>(ifFalseAttr, "ifFalse", "ifFalse", angleArray, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<AngleArrayData, MAngle>(ifTrueAttr, "ifTrue", "ifTrue", angleArray, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<AngleArrayData, MAngle>(outputAttr, "output", "output", angleArray, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectAngleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<MAngle> ifFalse = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, ifFalseAttr);
	std::vector<MAngle> ifTrue = inputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------