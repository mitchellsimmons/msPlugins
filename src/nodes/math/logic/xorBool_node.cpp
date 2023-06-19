#include "xorBool_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

XorBool::XorBool() {}
XorBool::~XorBool() {}

// ------ Attr ------
MObject XorBool::input1Attr;
MObject XorBool::input2Attr;
MObject XorBool::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType XorBool::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus XorBool::initialize()
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

MStatus XorBool::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	bool input1 = inputBoolValue(dataBlock, input1Attr);
	bool input2 = inputBoolValue(dataBlock, input2Attr);

	outputBoolValue(dataBlock, outputAttr, input1 ^ input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------