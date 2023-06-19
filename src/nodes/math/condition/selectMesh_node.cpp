#include "selectMesh_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SelectMesh::SelectMesh() {}
SelectMesh::~SelectMesh() {}

// ------ Attr ------
MObject SelectMesh::ifFalseAttr;
MObject SelectMesh::ifTrueAttr;
MObject SelectMesh::conditionAttr;
MObject SelectMesh::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SelectMesh::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SelectMesh::initialize()
{
	createMeshAttribute(ifFalseAttr, "ifFalse", "ifFalse", kDefaultPreset | kKeyable);
	createMeshAttribute(ifTrueAttr, "ifTrue", "ifTrue", kDefaultPreset | kKeyable);
	createBoolAttribute(conditionAttr, "condition", "condition", false, kDefaultPreset | kKeyable);
	createMeshAttribute(outputAttr, "output", "output", kReadOnlyPreset);

	addAttribute(ifFalseAttr);
	addAttribute(ifTrueAttr);
	addAttribute(conditionAttr);
	addAttribute(outputAttr);

	attributeAffects(ifFalseAttr, outputAttr);
	attributeAffects(ifTrueAttr, outputAttr);
	attributeAffects(conditionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SelectMesh::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MDataHandle ifFalseHandle = dataBlock.inputValue(ifFalseAttr);
	MDataHandle ifTrueHandle = dataBlock.inputValue(ifTrueAttr);
	bool condition = inputBoolValue(dataBlock, conditionAttr);

	MDataHandle	outputHandle = dataBlock.outputValue(outputAttr);
	outputHandle.setMObject(condition ? ifTrueHandle.asMesh() : ifFalseHandle.asMesh());
	outputHandle.setClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------