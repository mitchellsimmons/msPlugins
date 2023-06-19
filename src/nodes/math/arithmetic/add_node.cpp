#include "add_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Add::Add() {}
Add::~Add() {}

// ------ Attr ------
MObject Add::input1Attr;
MObject Add::input2Attr;
MObject Add::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Add::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Add::initialize()
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

MStatus Add::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);
	
	outputDoubleValue(dataBlock, outputAttr, input1 + input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------