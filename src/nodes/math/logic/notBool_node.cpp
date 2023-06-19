#include "notBool_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NotBool::NotBool() {}
NotBool::~NotBool() {}

// ------ Attr ------
MObject NotBool::inputAttr;
MObject NotBool::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NotBool::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NotBool::initialize()
{
	createBoolAttribute(inputAttr, "input", "input", false, kDefaultPreset | kKeyable);
	createBoolAttribute(outputAttr, "output", "output", false, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NotBool::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	bool input1 = inputBoolValue(dataBlock, inputAttr);

	outputBoolValue(dataBlock, outputAttr, !input1);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------