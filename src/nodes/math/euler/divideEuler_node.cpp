#include "divideEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideEuler::DivideEuler() {}
DivideEuler::~DivideEuler() {}

// ------ Attr ------
MObject DivideEuler::input1Attr;
MObject DivideEuler::input1XAttr;
MObject DivideEuler::input1YAttr;
MObject DivideEuler::input1ZAttr;
MObject DivideEuler::input2Attr;
MObject DivideEuler::outputAttr;
MObject DivideEuler::outputXAttr;
MObject DivideEuler::outputYAttr;
MObject DivideEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideEuler::initialize()
{
	MEulerRotation euler;

	createEulerAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", euler, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createEulerAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", euler, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DivideEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation input1 = inputEulerValue(dataBlock, input1Attr, input1XAttr, input1YAttr, input1ZAttr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	if (MRS::isEqual(input2, 0.0))
	{
		MGlobal::displayError("Undefined division by zero!");
		return MStatus::kFailure;
	}
	double multiplier = 1.0 / input2;

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, 
		{ input1.x * multiplier, input1.y * multiplier, input1.z * multiplier } );

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------