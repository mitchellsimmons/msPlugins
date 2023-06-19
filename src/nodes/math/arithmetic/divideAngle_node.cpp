#include "divideAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideAngle::DivideAngle() {}
DivideAngle::~DivideAngle() {}

// ------ Attr ------
MObject DivideAngle::input1Attr;
MObject DivideAngle::input2Attr;
MObject DivideAngle::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideAngle::initialize()
{
	createAngleAttribute(input1Attr, "input1", "input1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 1.0, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DivideAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input1 = inputAngleValue(dataBlock, input1Attr).asRadians();
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	if (MRS::isEqual(input2, 0.0))
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}

	outputAngleValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------