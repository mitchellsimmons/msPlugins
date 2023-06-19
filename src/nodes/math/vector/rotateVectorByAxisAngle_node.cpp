#include "rotateVectorByAxisAngle_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RotateVectorByAxisAngle::RotateVectorByAxisAngle() {}
RotateVectorByAxisAngle::~RotateVectorByAxisAngle() {}

// ------ Attr ------
MObject RotateVectorByAxisAngle::inputAttr;
MObject RotateVectorByAxisAngle::inputXAttr;
MObject RotateVectorByAxisAngle::inputYAttr;
MObject RotateVectorByAxisAngle::inputZAttr;
MObject RotateVectorByAxisAngle::unitAxisAttr;
MObject RotateVectorByAxisAngle::unitAxisXAttr;
MObject RotateVectorByAxisAngle::unitAxisYAttr;
MObject RotateVectorByAxisAngle::unitAxisZAttr;
MObject RotateVectorByAxisAngle::angleAttr;
MObject RotateVectorByAxisAngle::outputAttr;
MObject RotateVectorByAxisAngle::outputXAttr;
MObject RotateVectorByAxisAngle::outputYAttr;
MObject RotateVectorByAxisAngle::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RotateVectorByAxisAngle::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RotateVectorByAxisAngle::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(unitAxisAttr, unitAxisXAttr, unitAxisYAttr, unitAxisZAttr, "unitAxis", "unitAxis", vector, kDefaultPreset | kKeyable);
	createAngleAttribute(angleAttr, "angle", "angle", 0.0, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(unitAxisAttr);
	addAttribute(angleAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(unitAxisAttr, outputAttr);
	attributeAffects(angleAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RotateVectorByAxisAngle::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input = inputVectorValue(dataBlock, inputAttr);
	MVector axis = inputVectorValue(dataBlock, unitAxisAttr);
	double angle = inputAngleValue(dataBlock, angleAttr).asRadians();

	// Can either use a quaternion or Rodrigues
	outputVectorValue(dataBlock, outputAttr, MRS::rotateVectorByAxisAngle(input, axis, angle));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------