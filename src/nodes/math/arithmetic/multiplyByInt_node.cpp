#include "multiplyByInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyByInt::MultiplyByInt() {}
MultiplyByInt::~MultiplyByInt() {}

// ------ Attr ------
MObject MultiplyByInt::input1Attr;
MObject MultiplyByInt::input2Attr;
MObject MultiplyByInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyByInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyByInt::initialize()
{
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(input2Attr, "input2", "input2", 0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyByInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	int input2 = inputIntValue(dataBlock, input2Attr);

	outputDoubleValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------