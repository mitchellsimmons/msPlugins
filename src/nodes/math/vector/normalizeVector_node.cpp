#include "normalizeVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NormalizeVector::NormalizeVector() {}
NormalizeVector::~NormalizeVector() {}

// ------ Attr ------
MObject NormalizeVector::inputAttr;
MObject NormalizeVector::inputXAttr;
MObject NormalizeVector::inputYAttr;
MObject NormalizeVector::inputZAttr;
MObject NormalizeVector::outputAttr;
MObject NormalizeVector::outputXAttr;
MObject NormalizeVector::outputYAttr;
MObject NormalizeVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NormalizeVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NormalizeVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NormalizeVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);
	v.normalize();

	outputVectorValue(dataBlock, outputAttr, v);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------