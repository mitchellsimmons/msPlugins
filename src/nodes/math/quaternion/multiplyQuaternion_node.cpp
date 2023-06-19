#include "multiplyQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyQuaternion::MultiplyQuaternion() {}
MultiplyQuaternion::~MultiplyQuaternion() {}

// ------ Attr ------
MObject MultiplyQuaternion::input1Attr;
MObject MultiplyQuaternion::input1XAttr;
MObject MultiplyQuaternion::input1YAttr;
MObject MultiplyQuaternion::input1ZAttr;
MObject MultiplyQuaternion::input1WAttr;
MObject MultiplyQuaternion::input2Attr;
MObject MultiplyQuaternion::input2XAttr;
MObject MultiplyQuaternion::input2YAttr;
MObject MultiplyQuaternion::input2ZAttr;
MObject MultiplyQuaternion::input2WAttr;
MObject MultiplyQuaternion::outputAttr;
MObject MultiplyQuaternion::outputXAttr;
MObject MultiplyQuaternion::outputYAttr;
MObject MultiplyQuaternion::outputZAttr;
MObject MultiplyQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyQuaternion::initialize()
{
	MQuaternion quaternion;

	createQuaternionAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, input1WAttr, "input1", "input1", quaternion, kDefaultPreset | kKeyable);
	createQuaternionAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, input2WAttr, "input2", "input2", quaternion, kDefaultPreset | kKeyable);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, "output", "output", quaternion, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MQuaternion input1 = inputQuaternionValue(dataBlock, input1Attr, input1XAttr, input1YAttr, input1ZAttr, input1WAttr);
	MQuaternion input2 = inputQuaternionValue(dataBlock, input2Attr, input2XAttr, input2YAttr, input2ZAttr, input2WAttr);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------