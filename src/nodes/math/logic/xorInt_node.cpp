#include "xorInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

XorInt::XorInt() {}
XorInt::~XorInt() {}

// ------ Attr ------
MObject XorInt::input1Attr;
MObject XorInt::input2Attr;
MObject XorInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType XorInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus XorInt::initialize()
{
	createIntAttribute(input1Attr, "input1", "input1", false, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", false, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", false, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus XorInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	bool input1 = inputIntValue(dataBlock, input1Attr);
	bool input2 = inputIntValue(dataBlock, input2Attr);

	outputIntValue(dataBlock, outputAttr, input1 ^ input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------