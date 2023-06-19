#include "negateInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NegateInt::NegateInt() {}
NegateInt::~NegateInt() {}

// ------ Attr ------
MObject NegateInt::inputAttr;
MObject NegateInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NegateInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NegateInt::initialize()
{
	createIntAttribute(inputAttr, "input", "input", 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NegateInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input = inputIntValue(dataBlock, inputAttr);

	outputIntValue(dataBlock, outputAttr, -input);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------