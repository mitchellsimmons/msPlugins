#include "multiplyEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyEuler::MultiplyEuler() {}
MultiplyEuler::~MultiplyEuler() {}

// ------ Attr ------
MObject MultiplyEuler::input1Attr;
MObject MultiplyEuler::input1XAttr;
MObject MultiplyEuler::input1YAttr;
MObject MultiplyEuler::input1ZAttr;
MObject MultiplyEuler::input2Attr;
MObject MultiplyEuler::outputAttr;
MObject MultiplyEuler::outputXAttr;
MObject MultiplyEuler::outputYAttr;
MObject MultiplyEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyEuler::initialize()
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

MStatus MultiplyEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation input1 = inputEulerValue(dataBlock, input1Attr, input1XAttr, input1YAttr, input1ZAttr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr,
		{ input1.x * input2, input1.y * input2, input1.z * input2 });

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------