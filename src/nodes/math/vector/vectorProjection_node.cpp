#include "vectorProjection_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VectorProjection::VectorProjection() {}
VectorProjection::~VectorProjection() {}

// ------ Attr ------
MObject VectorProjection::input1Attr;
MObject VectorProjection::input1XAttr;
MObject VectorProjection::input1YAttr;
MObject VectorProjection::input1ZAttr;
MObject VectorProjection::input2Attr;
MObject VectorProjection::input2XAttr;
MObject VectorProjection::input2YAttr;
MObject VectorProjection::input2ZAttr;
MObject VectorProjection::outputAttr;
MObject VectorProjection::outputXAttr;
MObject VectorProjection::outputYAttr;
MObject VectorProjection::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VectorProjection::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VectorProjection::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus VectorProjection::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v1 = inputVectorValue(dataBlock, input1Attr);
	MVector v2 = inputVectorValue(dataBlock, input2Attr);

	// Project v1 onto unit v2
	v2.normalize();
	MVector vProj = (v1 * v2) * v2;

	outputVectorValue(dataBlock, outputAttr, vProj);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------