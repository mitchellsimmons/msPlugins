#include "smoothstepMatrix_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SmoothstepMatrix::SmoothstepMatrix() {}
SmoothstepMatrix::~SmoothstepMatrix() {}

// ------ Attr ------
MObject SmoothstepMatrix::input1Attr;
MObject SmoothstepMatrix::input2Attr;
MObject SmoothstepMatrix::tAttr;
MObject SmoothstepMatrix::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SmoothstepMatrix::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SmoothstepMatrix::initialize()
{
	MMatrix matrix;

	createMatrixAttribute(input1Attr, "input1", "input1", matrix, kDefaultPreset | kKeyable);
	createMatrixAttribute(input2Attr, "input2", "input2", matrix, kDefaultPreset | kKeyable);
	createDoubleAttribute(tAttr, "t", "t", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tAttr, 0.0, 1.0);
	createMatrixAttribute(outputAttr, "output", "output", matrix, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(tAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(tAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SmoothstepMatrix::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MMatrix input1Transform = inputMatrixValue(dataBlock, input1Attr);
	MMatrix input2Transform = inputMatrixValue(dataBlock, input2Attr);
	double t = inputDoubleValue(dataBlock, tAttr);
	t = t * t * (3 - 2 * t);

	MVector input1Translation, input2Translation;
	MVector input1Scale, input2Scale;
	MQuaternion input1Rotation, input2Rotation;
	MRS::decomposeMatrix(input1Transform, input1Translation, input1Rotation, input1Scale);
	MRS::decomposeMatrix(input2Transform, input2Translation, input2Rotation, input2Scale);

	MVector outputTranslation = input1Translation + (input2Translation - input1Translation) * t;
	MQuaternion outputRotation = slerp(input1Rotation, input2Rotation, t);
	MVector outputScale = input1Scale + (input2Scale - input1Scale) * t;

	outputMatrixValue(dataBlock, outputAttr, MRS::composeMatrix(outputTranslation, outputRotation, outputScale));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------