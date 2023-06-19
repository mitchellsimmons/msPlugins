#include "remapAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RemapAngle::RemapAngle() {}
RemapAngle::~RemapAngle() {}

// ------ Attr ------
MObject RemapAngle::inputAttr;
MObject RemapAngle::low1Attr;
MObject RemapAngle::high1Attr;
MObject RemapAngle::low2Attr;
MObject RemapAngle::high2Attr;
MObject RemapAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RemapAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RemapAngle::initialize()
{
	createAngleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(low1Attr, "low1", "low1", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(high1Attr, "high1", "high1", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(low2Attr, "low2", "low2", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(high2Attr, "high2", "high2", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(low1Attr);
	addAttribute(high1Attr);
	addAttribute(low2Attr);
	addAttribute(high2Attr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(low1Attr, outputAttr);
	attributeAffects(high1Attr, outputAttr);
	attributeAffects(low2Attr, outputAttr);
	attributeAffects(high2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RemapAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputAngleValue(dataBlock, inputAttr).asRadians();
	double low1 = inputAngleValue(dataBlock, low1Attr).asRadians();
	double high1 = inputAngleValue(dataBlock, high1Attr).asRadians();
	double low2 = inputAngleValue(dataBlock, low2Attr).asRadians();
	double high2 = inputAngleValue(dataBlock, high2Attr).asRadians();

	outputAngleValue(dataBlock, outputAttr, MRS::remap(input, low1, high1, low2, high2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------