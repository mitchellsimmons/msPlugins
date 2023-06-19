#include "selectSurface_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectSurface::SelectSurface() {}
SelectSurface::~SelectSurface() {}

// ------ Attr ------
MObject SelectSurface::ifFalseAttr;
MObject SelectSurface::ifTrueAttr;
MObject SelectSurface::conditionAttr;
MObject SelectSurface::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectSurface::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectSurface::initialize()
{
	createNurbsSurfaceAttribute(ifFalseAttr, "ifFalse", "ifFalse", kDefaultPreset | kKeyable);
	createNurbsSurfaceAttribute(ifTrueAttr, "ifTrue", "ifTrue", kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createNurbsSurfaceAttribute(outputAttr, "output", "output", kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectSurface::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle ifFalseHandle = dataBlock.inputValue(ifFalseAttr);
	MDataHandle ifTrueHandle = dataBlock.inputValue(ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	MDataHandle	outputHandle = dataBlock.outputValue(outputAttr);
	outputHandle.setMObject(condition ? ifTrueHandle.asNurbsSurface() : ifFalseHandle.asNurbsSurface());
	outputHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------