#include "multiply_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Multiply::Multiply() {}
Multiply::~Multiply() {}

// ------ Attr ------
MObject Multiply::input1Attr;
MObject Multiply::input2Attr;
MObject Multiply::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Multiply::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Multiply::initialize()
{
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Multiply::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputDoubleValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------