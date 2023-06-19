#include "distanceBetweenFrames_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DistanceBetweenFrames::DistanceBetweenFrames() {}
DistanceBetweenFrames::~DistanceBetweenFrames() {}

// ------ Attr ------
MObject DistanceBetweenFrames::input1Attr;
MObject DistanceBetweenFrames::input2Attr;
MObject DistanceBetweenFrames::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DistanceBetweenFrames::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DistanceBetweenFrames::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(input1Attr, "input1", "input1", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(input2Attr, "input2", "input2", matrix, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DistanceBetweenFrames::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input1 = inputMatrixValue(dataBlock, input1Attr);
	MMatrix input2 = inputMatrixValue(dataBlock, input2Attr);
	MVector position1 = MRS::extractTranslation(input1);
	MVector position2 = MRS::extractTranslation(input2);
	MVector delta = position1 - position2;

	outputDoubleValue(dataBlock, outputAttr, delta.length());

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------