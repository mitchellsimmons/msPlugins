#include "selectVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectVector::SelectVector() {}
SelectVector::~SelectVector() {}

// ------ Attr ------
MObject SelectVector::ifFalseAttr;
MObject SelectVector::ifFalseXAttr;
MObject SelectVector::ifFalseYAttr;
MObject SelectVector::ifFalseZAttr;
MObject SelectVector::ifTrueAttr;
MObject SelectVector::ifTrueXAttr;
MObject SelectVector::ifTrueYAttr;
MObject SelectVector::ifTrueZAttr;
MObject SelectVector::conditionAttr;
MObject SelectVector::outputAttr;
MObject SelectVector::outputXAttr;
MObject SelectVector::outputYAttr;
MObject SelectVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectVector::initialize()
{
	MVector vector;

	createVectorAttribute(ifFalseAttr, ifFalseXAttr, ifFalseYAttr, ifFalseZAttr, "ifFalse", "ifFalse", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(ifTrueAttr, ifTrueXAttr, ifTrueYAttr, ifTrueZAttr, "ifTrue", "ifTrue", vector, kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector ifFalse = inputVectorValue(dataBlock, ifFalseAttr);
	MVector ifTrue = inputVectorValue(dataBlock, ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	outputVectorValue(dataBlock, outputAttr, condition ? ifTrue : ifFalse);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------