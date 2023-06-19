#include "selectQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectQuaternion::SelectQuaternion() {}
SelectQuaternion::~SelectQuaternion() {}

// ------ Attr ------
MObject SelectQuaternion::ifFalseAttr;
MObject SelectQuaternion::ifFalseXAttr;
MObject SelectQuaternion::ifFalseYAttr;
MObject SelectQuaternion::ifFalseZAttr;
MObject SelectQuaternion::ifFalseWAttr;
MObject SelectQuaternion::ifTrueAttr;
MObject SelectQuaternion::ifTrueXAttr;
MObject SelectQuaternion::ifTrueYAttr;
MObject SelectQuaternion::ifTrueZAttr;
MObject SelectQuaternion::ifTrueWAttr;
MObject SelectQuaternion::conditionAttr;
MObject SelectQuaternion::outputAttr;
MObject SelectQuaternion::outputXAttr;
MObject SelectQuaternion::outputYAttr;
MObject SelectQuaternion::outputZAttr;
MObject SelectQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectQuaternion::initialize()
{
	MQuaternion quaternion;

	createQuaternionAttribute(ifFalseAttr, ifFalseXAttr, ifFalseYAttr, ifFalseZAttr, ifFalseWAttr, 
		"ifFalse", "ifFalse", quaternion, kDefaultPreset | kKeyable);
	createQuaternionAttribute(ifTrueAttr, ifTrueXAttr, ifTrueYAttr, ifTrueZAttr, ifTrueWAttr, 
		"ifTrue", "ifTrue", quaternion, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, 
		"output", "output", quaternion, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MQuaternion ifFalse = inputQuaternionValue(dataBlock, ifFalseAttr, ifFalseXAttr, ifFalseYAttr, ifFalseZAttr, ifFalseWAttr);
	MQuaternion ifTrue = inputQuaternionValue(dataBlock, ifTrueAttr, ifTrueXAttr, ifTrueYAttr, ifTrueZAttr, ifTrueWAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}
