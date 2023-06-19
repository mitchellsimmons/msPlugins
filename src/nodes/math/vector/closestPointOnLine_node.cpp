#include "closestPointOnLine_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ClosestPointOnLine::ClosestPointOnLine() {}
ClosestPointOnLine::~ClosestPointOnLine() {}

// ------ Attr ------
MObject ClosestPointOnLine::targetAttr;
MObject ClosestPointOnLine::targetXAttr;
MObject ClosestPointOnLine::targetYAttr;
MObject ClosestPointOnLine::targetZAttr;
MObject ClosestPointOnLine::pointOnLine1Attr;
MObject ClosestPointOnLine::pointOnLine1XAttr;
MObject ClosestPointOnLine::pointOnLine1YAttr;
MObject ClosestPointOnLine::pointOnLine1ZAttr;
MObject ClosestPointOnLine::pointOnLine2Attr;
MObject ClosestPointOnLine::pointOnLine2XAttr;
MObject ClosestPointOnLine::pointOnLine2YAttr;
MObject ClosestPointOnLine::pointOnLine2ZAttr;
MObject ClosestPointOnLine::outputAttr;
MObject ClosestPointOnLine::outputXAttr;
MObject ClosestPointOnLine::outputYAttr;
MObject ClosestPointOnLine::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ClosestPointOnLine::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ClosestPointOnLine::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(targetAttr, targetXAttr, targetYAttr, targetZAttr, "target", "target", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(pointOnLine1Attr, pointOnLine1XAttr, pointOnLine1YAttr, pointOnLine1ZAttr, "pointOnLine1", "pointOnLine1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(pointOnLine2Attr, pointOnLine2XAttr, pointOnLine2YAttr, pointOnLine2ZAttr, "pointOnLine2", "pointOnLine2", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(targetAttr);
	addAttribute(pointOnLine1Attr);
	addAttribute(pointOnLine2Attr);
	addAttribute(outputAttr);

	attributeAffects(targetAttr, outputAttr);
	attributeAffects(pointOnLine1Attr, outputAttr);
	attributeAffects(pointOnLine2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus ClosestPointOnLine::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector target = inputVectorValue(dataBlock, targetAttr);
	MVector pointOnLine1 = inputVectorValue(dataBlock, pointOnLine1Attr);
	MVector pointOnLine2 = inputVectorValue(dataBlock, pointOnLine2Attr);

	MVector line = pointOnLine2 - pointOnLine1;
	MVector pointOnLine1ToTarget = target - pointOnLine1;
	double dot = pointOnLine1ToTarget * line;
	double lineLength = line.length();
	double projectedLength = 0.0;
	if (!MRS::isEqual(lineLength, 0.0))
	{
		projectedLength = dot / lineLength;
		line /= lineLength;
	}
	MVector projected = projectedLength * line;
	MVector closestPoint = pointOnLine1 + projected;

	outputVectorValue(dataBlock, outputAttr, closestPoint);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------