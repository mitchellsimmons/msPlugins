#include "acos_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Acos::Acos() {}
Acos::~Acos() {}

// ------ Attr ------
MObject Acos::inputAttr;
MObject Acos::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Acos::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Acos::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	setMinMax(inputAttr, -1.0, 1.0);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Acos::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputAngleValue(dataBlock, outputAttr, std::acos(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------