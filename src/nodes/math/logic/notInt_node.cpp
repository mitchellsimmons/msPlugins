#include "notInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NotInt::NotInt() {}
NotInt::~NotInt() {}

// ------ Attr ------
MObject NotInt::inputAttr;
MObject NotInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NotInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NotInt::initialize()
{
	createIntAttribute(inputAttr, "input", "input", false, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", false, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NotInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input1 = inputIntValue(dataBlock, inputAttr);

	outputIntValue(dataBlock, outputAttr, !input1);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------