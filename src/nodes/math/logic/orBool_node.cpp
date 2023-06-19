#include "orBool_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

OrBool::OrBool() {}
OrBool::~OrBool() {}

// ------ Attr ------
MObject OrBool::input1Attr;
MObject OrBool::input2Attr;
MObject OrBool::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType OrBool::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus OrBool::initialize()
{
	createBoolAttribute(input1Attr, "input1", "input1", false, kDefaultPreset | kKeyable);
	createBoolAttribute(input2Attr, "input2", "input2", false, kDefaultPreset | kKeyable);
	createBoolAttribute(outputAttr, "output", "output", false, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus OrBool::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	bool input1 = inputBoolValue(dataBlock, input1Attr);
	bool input2 = inputBoolValue(dataBlock, input2Attr);

	outputBoolValue(dataBlock, outputAttr, input1 || input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------