#include "asin_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Asin::Asin() {}
Asin::~Asin() {}

// ------ Attr ------
MObject Asin::inputAttr;
MObject Asin::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Asin::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Asin::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	setMinMax(inputAttr, -1.0, 1.0);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Asin::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);

	outputAngleValue(dataBlock, outputAttr, std::asin(input));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------