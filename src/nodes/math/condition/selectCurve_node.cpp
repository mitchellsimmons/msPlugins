#include "selectCurve_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectCurve::SelectCurve() {}
SelectCurve::~SelectCurve() {}

// ------ Attr ------
MObject SelectCurve::ifFalseAttr;
MObject SelectCurve::ifTrueAttr;
MObject SelectCurve::conditionAttr;
MObject SelectCurve::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectCurve::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectCurve::initialize()
{
	createNurbsCurveAttribute(ifFalseAttr, "ifFalse", "ifFalse", kDefaultPreset | kKeyable);
	createNurbsCurveAttribute(ifTrueAttr, "ifTrue", "ifTrue", kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createNurbsCurveAttribute(outputAttr, "output", "output", kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectCurve::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle ifFalseHandle = dataBlock.inputValue(ifFalseAttr);
	MDataHandle ifTrueHandle = dataBlock.inputValue(ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	MDataHandle	outputHandle = dataBlock.outputValue(outputAttr);
	outputHandle.setMObject(condition ? ifTrueHandle.asNurbsCurve() : ifFalseHandle.asNurbsCurve());
	outputHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------