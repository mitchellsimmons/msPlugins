#include "divideInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideInt::DivideInt() {}
DivideInt::~DivideInt() {}

// ------ Attr ------
MObject DivideInt::input1Attr;
MObject DivideInt::input2Attr;
MObject DivideInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideInt::initialize()
{
	createIntAttribute(input1Attr, "input1", "input1", 0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 1, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DivideInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input1 = inputIntValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);

	if (input2 == 0)
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputIntValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------