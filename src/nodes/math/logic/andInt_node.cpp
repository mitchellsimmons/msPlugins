#include "andInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AndInt::AndInt() {}
AndInt::~AndInt() {}

// ------ Attr ------
MObject AndInt::input1Attr;
MObject AndInt::input2Attr;
MObject AndInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AndInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AndInt::initialize()
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

MStatus AndInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input1 = inputIntValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);

	outputIntValue(dataBlock, outputAttr, input1 && input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------