#include "addInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AddInt::AddInt() {}
AddInt::~AddInt() {}

// ------ Attr ------
MObject AddInt::input1Attr;
MObject AddInt::input2Attr;
MObject AddInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AddInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AddInt::initialize()
{
	createIntAttribute(input1Attr, "input1", "input1", 0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AddInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input1 = inputIntValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);

	outputIntValue(dataBlock, outputAttr, input1 + input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------