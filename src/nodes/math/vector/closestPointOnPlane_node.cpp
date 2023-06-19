#include "closestPointOnPlane_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ClosestPointOnPlane::ClosestPointOnPlane() {}
ClosestPointOnPlane::~ClosestPointOnPlane() {}

// ------ Attr ------
MObject ClosestPointOnPlane::targetAttr;
MObject ClosestPointOnPlane::targetXAttr;
MObject ClosestPointOnPlane::targetYAttr;
MObject ClosestPointOnPlane::targetZAttr;
MObject ClosestPointOnPlane::pointOnPlaneAttr;
MObject ClosestPointOnPlane::pointOnPlaneXAttr;
MObject ClosestPointOnPlane::pointOnPlaneYAttr;
MObject ClosestPointOnPlane::pointOnPlaneZAttr;
MObject ClosestPointOnPlane::normalToPlaneAttr;
MObject ClosestPointOnPlane::normalToPlaneXAttr;
MObject ClosestPointOnPlane::normalToPlaneYAttr;
MObject ClosestPointOnPlane::normalToPlaneZAttr;
MObject ClosestPointOnPlane::outputAttr;
MObject ClosestPointOnPlane::outputXAttr;
MObject ClosestPointOnPlane::outputYAttr;
MObject ClosestPointOnPlane::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ClosestPointOnPlane::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ClosestPointOnPlane::initialize()
{
	MVector vector = MVector::zero;
	MVector normal = MVector::yAxis;

	createVectorAttribute(targetAttr, targetXAttr, targetYAttr, targetZAttr, "target", "target", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(pointOnPlaneAttr, pointOnPlaneXAttr, pointOnPlaneYAttr, pointOnPlaneZAttr, "pointOnPlane", "pointOnPlane", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(normalToPlaneAttr, normalToPlaneXAttr, normalToPlaneYAttr, normalToPlaneZAttr, "normalToPlane", "normalToPlane", normal, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(targetAttr);
	addAttribute(pointOnPlaneAttr);
	addAttribute(normalToPlaneAttr);
	addAttribute(outputAttr);

	attributeAffects(targetAttr, outputAttr);
	attributeAffects(pointOnPlaneAttr, outputAttr);
	attributeAffects(normalToPlaneAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ClosestPointOnPlane::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector target = inputVectorValue(dataBlock, targetAttr);
	MVector pointOnPlane = inputVectorValue(dataBlock, pointOnPlaneAttr);
	MVector normalToPlane = inputVectorValue(dataBlock, normalToPlaneAttr);

	// Stabalize the normal if a zero vector is given
	double normalToPlaneLength = normalToPlane.length();
	if (MRS::isEqual(normalToPlaneLength, 0.0))
	{
		normalToPlane = { 0.0, 1.0, 0.0 };
		normalToPlaneLength = 1.0;
	}

	// Project onto the normal, a vector from the point on the plane to the target, then subtract the projection from the target
	MVector pointOnPlaneToTarget = target - pointOnPlane;
	double dot = normalToPlane * pointOnPlaneToTarget;
	double projectedLength = dot / normalToPlaneLength;
	normalToPlane /= normalToPlaneLength;
	MVector projOnNormal = projectedLength * normalToPlane;
	MVector closestPoint = target - projOnNormal;

	outputVectorValue(dataBlock, outputAttr, closestPoint);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------