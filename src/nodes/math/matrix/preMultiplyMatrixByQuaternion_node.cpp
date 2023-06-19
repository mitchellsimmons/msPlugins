#include "preMultiplyMatrixByQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

PreMultiplyMatrixByQuaternion::PreMultiplyMatrixByQuaternion() : MPxNode(), NodeHelper() {}
PreMultiplyMatrixByQuaternion::~PreMultiplyMatrixByQuaternion() {}

// ------ Attr ------
MObject PreMultiplyMatrixByQuaternion::quaternionAttr;
MObject PreMultiplyMatrixByQuaternion::quaternionXAttr;
MObject PreMultiplyMatrixByQuaternion::quaternionYAttr;
MObject PreMultiplyMatrixByQuaternion::quaternionZAttr;
MObject PreMultiplyMatrixByQuaternion::quaternionWAttr;
MObject PreMultiplyMatrixByQuaternion::inputAttr;
MObject PreMultiplyMatrixByQuaternion::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType PreMultiplyMatrixByQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus PreMultiplyMatrixByQuaternion::initialize()
{
	MMatrix matrix;
	MQuaternion quaternion;

	createQuaternionAttribute(quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr,
		"quaternion", "quaternion", quaternion, kDefaultPreset | kKeyable);
	createMatrixAttribute(inputAttr, "input", "input", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(quaternionAttr);
	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(quaternionAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus PreMultiplyMatrixByQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix matrix = inputMatrixValue(dataBlock, inputAttr);
	MQuaternion quaternion = inputQuaternionValue(dataBlock, quaternionAttr, quaternionXAttr, quaternionYAttr, quaternionZAttr, quaternionWAttr);

	outputMatrixValue(dataBlock, outputAttr, quaternion.asMatrix() * matrix);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------