#include "cartesianToPolar_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CartesianToPolar::CartesianToPolar() {}
CartesianToPolar::~CartesianToPolar() {}

// ------ Attr ------
MObject CartesianToPolar::xAttr;
MObject CartesianToPolar::yAttr;
MObject CartesianToPolar::outputAngleAttr;
MObject CartesianToPolar::outputRadiusAttr;

// ------ MPxNode ------
MPxNode::SchedulingType CartesianToPolar::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus CartesianToPolar::initialize()
{
	MVector vector = MVector::zero;

	createDoubleAttribute(xAttr, "x", "x", 1.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(yAttr, "y", "y", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAngleAttr, "outputAngle", "outputAngle", 0.0, kReadOnlyPreset);
	createDoubleAttribute(outputRadiusAttr, "outputRadius", "outputRadius", 1.0, kReadOnlyPreset);

	addAttribute(xAttr);
	addAttribute(yAttr);
	addAttribute(outputAngleAttr);
	addAttribute(outputRadiusAttr);

	attributeAffects(xAttr, outputAngleAttr);
	attributeAffects(yAttr, outputAngleAttr);
	attributeAffects(xAttr, outputRadiusAttr);
	attributeAffects(yAttr, outputRadiusAttr);

	return MStatus::kSuccess;
}

MStatus CartesianToPolar::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	double x = inputDoubleValue(dataBlock, xAttr);
	double y = inputDoubleValue(dataBlock, yAttr);

	if (plug == outputAngleAttr)
	{
		if (MRS::isEqual(y, 0.0) && MRS::isEqual(x, 0.0))
		{
			MGlobal::displayError("Domain error: x and y coordinates cannot both equal 0 as proportion y/x is undefined.");
			return MStatus::kFailure;
		}

		outputAngleValue(dataBlock, outputAngleAttr, std::atan2(y, x));
	}
	else if (plug == outputRadiusAttr)
		outputDoubleValue(dataBlock, outputRadiusAttr, std::sqrt(x * x + y * y));
	else
		return MStatus::kUnknownParameter;

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------