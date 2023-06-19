#include "divideByInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideByInt::DivideByInt() {}
DivideByInt::~DivideByInt() {}

// ------ Attr ------
MObject DivideByInt::input1Attr;
MObject DivideByInt::input2Attr;
MObject DivideByInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideByInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideByInt::initialize()
{
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 1, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DivideByInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);

	if (input2 == 0)
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputDoubleValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------