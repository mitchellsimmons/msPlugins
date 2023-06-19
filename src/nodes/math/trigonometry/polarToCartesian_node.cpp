#include "polarToCartesian_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PolarToCartesian::PolarToCartesian() {}
PolarToCartesian::~PolarToCartesian() {}

// ------ Attr ------
MObject PolarToCartesian::angleAttr;
MObject PolarToCartesian::radiusAttr;
MObject PolarToCartesian::outputXAttr;
MObject PolarToCartesian::outputYAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PolarToCartesian::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PolarToCartesian::initialize()
{
	MVector vector = MVector::zero;

	createAngleAttribute(angleAttr, "angle", "angle", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(radiusAttr, "radius", "radius", 1.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputXAttr, "outputX", "outputX", 1.0, kReadOnlyPreset);
	createDoubleAttribute(outputYAttr, "outputY", "outputY", 0.0, kReadOnlyPreset);

	addAttribute(angleAttr);
	addAttribute(radiusAttr);
	addAttribute(outputXAttr);
	addAttribute(outputYAttr);

	attributeAffects(angleAttr, outputXAttr);
	attributeAffects(radiusAttr, outputXAttr);
	attributeAffects(angleAttr, outputYAttr);
	attributeAffects(radiusAttr, outputYAttr);

	return MStatus::kSuccess;
}

MStatus PolarToCartesian::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	double angle = inputAngleValue(dataBlock, angleAttr).asRadians();
	double radius = inputDoubleValue(dataBlock, radiusAttr);

	if (plug == outputXAttr)
		outputDoubleValue(dataBlock, outputXAttr, std::cos(angle) * radius);
	else if (plug == outputYAttr)
		outputDoubleValue(dataBlock, outputYAttr, std::sin(angle) * radius);
	else
		return MStatus::kUnknownParameter;

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------