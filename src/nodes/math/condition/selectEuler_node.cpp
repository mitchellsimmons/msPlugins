#include "selectEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectEuler::SelectEuler() {}
SelectEuler::~SelectEuler() {}

// ------ Attr ------
MObject SelectEuler::ifFalseAttr;
MObject SelectEuler::ifFalseXAttr;
MObject SelectEuler::ifFalseYAttr;
MObject SelectEuler::ifFalseZAttr;
MObject SelectEuler::ifTrueAttr;
MObject SelectEuler::ifTrueXAttr;
MObject SelectEuler::ifTrueYAttr;
MObject SelectEuler::ifTrueZAttr;
MObject SelectEuler::conditionAttr;
MObject SelectEuler::outputAttr;
MObject SelectEuler::outputXAttr;
MObject SelectEuler::outputYAttr;
MObject SelectEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectEuler::initialize()
{
	MEulerRotation euler;

	createEulerAttribute(ifFalseAttr, ifFalseXAttr, ifFalseYAttr, ifFalseZAttr, "ifFalse", "ifFalse", euler, kDefaultPreset | kKeyable);
	createEulerAttribute(ifTrueAttr, ifTrueXAttr, ifTrueYAttr, ifTrueZAttr, "ifTrue", "ifTrue", euler, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createEulerAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", euler, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation ifFalse = inputEulerValue(dataBlock, ifFalseAttr, ifFalseXAttr, ifFalseYAttr, ifFalseZAttr);
	MEulerRotation ifTrue = inputEulerValue(dataBlock, ifTrueAttr, ifTrueXAttr, ifTrueYAttr, ifTrueZAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}
