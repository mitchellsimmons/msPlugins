#include "divide_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Divide::Divide() {}
Divide::~Divide() {}

// ------ Attr ------
MObject Divide::input1Attr;
MObject Divide::input2Attr;
MObject Divide::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Divide::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Divide::initialize()
{
	createDoubleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 1.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Divide::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputDoubleValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	if (MRS::isEqual(input2, 0.0))
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputDoubleValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------