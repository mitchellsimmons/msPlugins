#include "postMultiplyMatrixByQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PostMultiplyMatrixByQuaternion::PostMultiplyMatrixByQuaternion() : MPxNode(), NodeHelper() {}
PostMultiplyMatrixByQuaternion::~PostMultiplyMatrixByQuaternion() {}

// ------ Attr ------
MObject PostMultiplyMatrixByQuaternion::inputAttr;
MObject PostMultiplyMatrixByQuaternion::quaternionAttr;
MObject PostMultiplyMatrixByQuaternion::quaternionXAttr;
MObject PostMultiplyMatrixByQuaternion::quaternionYAttr;
MObject PostMultiplyMatrixByQuaternion::quaternionZAttr;
MObject PostMultiplyMatrixByQuaternion::quaternionWAttr;
MObject PostMultiplyMatrixByQuaternion::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PostMultiplyMatrixByQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PostMultiplyMatrixByQuaternion::initialize()
{
	MMatrix matrix;
	MQuaternion quaternion;

	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createQuaternionAttribute(quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr, 
		"quaternion", "quaternion", quaternion, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(quaternionAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(quaternionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PostMultiplyMatrixByQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MQuaternion quaternion = inputQuaternionValue(dataBlock, quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr);

	outputMatrixValue(dataBlock, outputAttr, matrix * quaternion.asMatrix());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------