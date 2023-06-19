#include "multiplyAngleByInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyAngleByInt::MultiplyAngleByInt() {}
MultiplyAngleByInt::~MultiplyAngleByInt() {}

// ------ Attr ------
MObject MultiplyAngleByInt::input1Attr;
MObject MultiplyAngleByInt::input2Attr;
MObject MultiplyAngleByInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyAngleByInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyAngleByInt::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyAngleByInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	int input2 = inputIntValue(dataBlock, input2Attr);

	outputAngleValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------