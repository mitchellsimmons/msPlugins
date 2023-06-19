#include "lerp_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Lerp::Lerp() {}
Lerp::~Lerp() {}

// ------ Attr ------
MObject Lerp::input1Attr;
MObject Lerp::input2Attr;
MObject Lerp::tAttr;
MObject Lerp::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Lerp::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Lerp::initialize()
{
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(tAttr, "t", "t", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tAttr, 0.0, 1.0);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(tAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(tAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Lerp::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);
	double t = inputDoubleValue(dataBlock, tAttr);

	outputDoubleValue(dataBlock, outputAttr, input1 + (input2 - input1) * t);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------