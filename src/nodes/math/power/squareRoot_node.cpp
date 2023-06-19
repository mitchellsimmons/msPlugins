#include "squareRoot_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SquareRoot::SquareRoot() {}
SquareRoot::~SquareRoot() {}

// ------ Attr ------
MObject SquareRoot::inputAttr;
MObject SquareRoot::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SquareRoot::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SquareRoot::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SquareRoot::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputDoubleValue(dataBlock, outputAttr, std::sqrt(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------