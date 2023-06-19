#include "vectorPlaneProjection_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VectorPlaneProjection::VectorPlaneProjection() {}
VectorPlaneProjection::~VectorPlaneProjection() {}

// ------ Attr ------
MObject VectorPlaneProjection::inputAttr;
MObject VectorPlaneProjection::inputXAttr;
MObject VectorPlaneProjection::inputYAttr;
MObject VectorPlaneProjection::inputZAttr;
MObject VectorPlaneProjection::normalToPlaneAttr;
MObject VectorPlaneProjection::normalToPlaneXAttr;
MObject VectorPlaneProjection::normalToPlaneYAttr;
MObject VectorPlaneProjection::normalToPlaneZAttr;
MObject VectorPlaneProjection::outputAttr;
MObject VectorPlaneProjection::outputXAttr;
MObject VectorPlaneProjection::outputYAttr;
MObject VectorPlaneProjection::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VectorPlaneProjection::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VectorPlaneProjection::initialize()
{
	MVector vector = MVector::zero;
	MVector normal = MVector::yAxis;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(normalToPlaneAttr, normalToPlaneXAttr, normalToPlaneYAttr, normalToPlaneZAttr, "normalToPlane", "normalToPlane", normal, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(normalToPlaneAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(normalToPlaneAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus VectorPlaneProjection::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);
	MVector n = inputVectorValue(dataBlock, normalToPlaneAttr);

	// Stabalize the normal if a zero vector is given
	double nLength = n.length();
	if (MRS::isEqual(nLength, 0.0))
	{
		n = { 0.0, 1.0, 0.0 };
		nLength = 1.0;
	}
	else
		n /= nLength;

	MVector vProj = v - (v * n) * n;

	outputVectorValue(dataBlock, outputAttr, vProj);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------