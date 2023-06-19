#include "subtractVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SubtractVector::SubtractVector() {}
SubtractVector::~SubtractVector() {}

// ------ Attr ------
MObject SubtractVector::input1Attr;
MObject SubtractVector::input1XAttr;
MObject SubtractVector::input1YAttr;
MObject SubtractVector::input1ZAttr;
MObject SubtractVector::input2Attr;
MObject SubtractVector::input2XAttr;
MObject SubtractVector::input2YAttr;
MObject SubtractVector::input2ZAttr;
MObject SubtractVector::outputAttr;
MObject SubtractVector::outputXAttr;
MObject SubtractVector::outputYAttr;
MObject SubtractVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SubtractVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SubtractVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SubtractVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 - input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------