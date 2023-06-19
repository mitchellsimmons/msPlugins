#include "min_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Min::Min() {}
Min::~Min() {}

// ------ Attr ------
MObject Min::input1Attr;
MObject Min::input2Attr;
MObject Min::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Min::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Min::initialize()
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

MStatus Min::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputDoubleValue(dataBlock, outputAttr, std::min(input1, input2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------