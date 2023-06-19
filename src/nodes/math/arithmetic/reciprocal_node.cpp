#include "reciprocal_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Reciprocal::Reciprocal() {}
Reciprocal::~Reciprocal() {}

// ------ Attr ------
MObject Reciprocal::inputAttr;
MObject Reciprocal::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Reciprocal::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Reciprocal::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 1.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 1.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Reciprocal::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	if (MRS::isEqual(input, 0.0))
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputDoubleValue(dataBlock, outputAttr, 1.0 / input);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------