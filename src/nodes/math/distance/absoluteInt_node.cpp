#include "absoluteInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AbsoluteInt::AbsoluteInt() {}
AbsoluteInt::~AbsoluteInt() {}

// ------ Attr ------
MObject AbsoluteInt::inputAttr;
MObject AbsoluteInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AbsoluteInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AbsoluteInt::initialize()
{
	createIntAttribute(inputAttr, "input", "input", 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AbsoluteInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input = inputIntValue(dataBlock, inputAttr);

	outputIntValue(dataBlock, outputAttr, std::abs(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------