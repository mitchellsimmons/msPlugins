#include "slerpQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SlerpQuaternion::SlerpQuaternion() {}
SlerpQuaternion::~SlerpQuaternion() {}

// ------ Attr ------
MObject SlerpQuaternion::input1Attr;
MObject SlerpQuaternion::input1XAttr;
MObject SlerpQuaternion::input1YAttr;
MObject SlerpQuaternion::input1ZAttr;
MObject SlerpQuaternion::input1WAttr;
MObject SlerpQuaternion::input2Attr;
MObject SlerpQuaternion::input2XAttr;
MObject SlerpQuaternion::input2YAttr;
MObject SlerpQuaternion::input2ZAttr;
MObject SlerpQuaternion::input2WAttr;
MObject SlerpQuaternion::tAttr;
MObject SlerpQuaternion::outputAttr;
MObject SlerpQuaternion::outputXAttr;
MObject SlerpQuaternion::outputYAttr;
MObject SlerpQuaternion::outputZAttr;
MObject SlerpQuaternion::outputWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SlerpQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SlerpQuaternion::initialize()
{
	MQuaternion quaternion;

	createQuaternionAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, input1WAttr, 
		"input1", "input1", quaternion, kDefaultPreset | kKeyable);
	createQuaternionAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, input2WAttr, 
		"input2", "input2", quaternion, kDefaultPreset | kKeyable);
	createDoubleAttribute(tAttr, "t", "t", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tAttr, 0.0, 1.0);
	createQuaternionAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, 
		"output", "output", quaternion, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(tAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(tAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SlerpQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputWAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MQuaternion input1 = inputQuaternionValue(dataBlock, input1Attr, input1XAttr, input1YAttr, input1ZAttr, input1WAttr);
	MQuaternion input2 = inputQuaternionValue(dataBlock, input2Attr, input2XAttr, input2YAttr, input2ZAttr, input2WAttr);
	double t = inputDoubleValue(dataBlock, tAttr);

	outputQuaternionValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, outputWAttr, slerp(input1, input2, t));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------