#include "reflectVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ReflectVector::ReflectVector() {}
ReflectVector::~ReflectVector() {}

// ------ Attr ------
MObject ReflectVector::inputAttr;
MObject ReflectVector::inputXAttr;
MObject ReflectVector::inputYAttr;
MObject ReflectVector::inputZAttr;
MObject ReflectVector::unitReflectionAxisAttr;
MObject ReflectVector::unitReflectionAxisXAttr;
MObject ReflectVector::unitReflectionAxisYAttr;
MObject ReflectVector::unitReflectionAxisZAttr;
MObject ReflectVector::outputAttr;
MObject ReflectVector::outputXAttr;
MObject ReflectVector::outputYAttr;
MObject ReflectVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ReflectVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ReflectVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(unitReflectionAxisAttr, unitReflectionAxisXAttr, unitReflectionAxisYAttr, unitReflectionAxisZAttr,
		"unitReflectionAxis", "unitReflectionAxis", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(unitReflectionAxisAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(unitReflectionAxisAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ReflectVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);
	MVector reflectionAxis = inputVectorValue(dataBlock, unitReflectionAxisAttr);

	reflectionAxis.normalize();
	MVector vProj = (v * reflectionAxis) * reflectionAxis;
	MVector vPrime = 2 * vProj - v;

	outputVectorValue(dataBlock, outputAttr, vPrime);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------