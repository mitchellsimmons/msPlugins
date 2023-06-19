#include "atan2_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Atan2::Atan2() {}
Atan2::~Atan2() {}

// ------ Attr ------
MObject Atan2::yAttr;
MObject Atan2::xAttr;
MObject Atan2::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Atan2::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Atan2::initialize()
{
	createDoubleAttribute(yAttr, "y", "y", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(xAttr, "x", "x", 1.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(yAttr);
	addAttribute(xAttr);
	addAttribute(outputAttr);

	attributeAffects(yAttr, outputAttr);
	attributeAffects(xAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus Atan2::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double y = inputDoubleValue(dataBlock, yAttr);
	double x = inputDoubleValue(dataBlock, xAttr);

	if (MRS::isEqual(y, 0.0) && MRS::isEqual(x, 0.0))
	{
		MGlobal::displayError("Domain error: x and y coordinates cannot both equal 0 as proportion y/x is undefined.");
		return MStatus::kFailure;
	}

	outputAngleValue(dataBlock, outputAttr, std::atan2(y, x));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------