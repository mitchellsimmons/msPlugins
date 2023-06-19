#include "divideAngleByInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideAngleByInt::DivideAngleByInt() {}
DivideAngleByInt::~DivideAngleByInt() {}

// ------ Attr ------
MObject DivideAngleByInt::input1Attr;
MObject DivideAngleByInt::input2Attr;
MObject DivideAngleByInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideAngleByInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideAngleByInt::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 1, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DivideAngleByInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	int input2 = inputIntValue(dataBlock, input2Attr);

	if (input2 == 0)
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputAngleValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------