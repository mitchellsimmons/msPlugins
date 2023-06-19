#include "rotateVectorByQuaternion_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RotateVectorByQuaternion::RotateVectorByQuaternion() {}
RotateVectorByQuaternion::~RotateVectorByQuaternion() {}

// ------ Attr ------
MObject RotateVectorByQuaternion::inputAttr;
MObject RotateVectorByQuaternion::inputXAttr;
MObject RotateVectorByQuaternion::inputYAttr;
MObject RotateVectorByQuaternion::inputZAttr;
MObject RotateVectorByQuaternion::unitQuaternionAttr;
MObject RotateVectorByQuaternion::unitQuaternionXAttr;
MObject RotateVectorByQuaternion::unitQuaternionYAttr;
MObject RotateVectorByQuaternion::unitQuaternionZAttr;
MObject RotateVectorByQuaternion::unitQuaternionWAttr;
MObject RotateVectorByQuaternion::outputAttr;
MObject RotateVectorByQuaternion::outputXAttr;
MObject RotateVectorByQuaternion::outputYAttr;
MObject RotateVectorByQuaternion::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RotateVectorByQuaternion::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RotateVectorByQuaternion::initialize()
{
	MVector vector = MVector::zero;
	MQuaternion quaternion = MQuaternion::identity;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createQuaternionAttribute(unitQuaternionAttr, unitQuaternionXAttr, unitQuaternionYAttr, unitQuaternionZAttr, unitQuaternionWAttr, 
		"unitQuaternion", "unitQuaternion", quaternion, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(unitQuaternionAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(unitQuaternionAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RotateVectorByQuaternion::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector vInput1 = inputVectorValue(dataBlock, inputAttr);
	MQuaternion qInput2 = inputQuaternionValue(dataBlock, unitQuaternionAttr, unitQuaternionXAttr, unitQuaternionYAttr, unitQuaternionZAttr, unitQuaternionWAttr);

	/*MQuaternion qInput1{ vInput1.x, vInput1.y, vInput1.z, 0.0 };
	MQuaternion qTransformed = qInput2.conjugate() * qInput1 * qInput2;
	MVector vPrime{ qTransformed.x, qTransformed.y, qTransformed.z };*/

	outputVectorValue(dataBlock, outputAttr, MRS::rotateVectorByQuaternion(vInput1, qInput2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------