#include "smoothstepVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SmoothstepVector::SmoothstepVector() {}
SmoothstepVector::~SmoothstepVector() {}

// ------ Attr ------
MObject SmoothstepVector::input1Attr;
MObject SmoothstepVector::input1XAttr;
MObject SmoothstepVector::input1YAttr;
MObject SmoothstepVector::input1ZAttr;
MObject SmoothstepVector::input2Attr;
MObject SmoothstepVector::input2XAttr;
MObject SmoothstepVector::input2YAttr;
MObject SmoothstepVector::input2ZAttr;
MObject SmoothstepVector::tAttr;
MObject SmoothstepVector::outputAttr;
MObject SmoothstepVector::outputXAttr;
MObject SmoothstepVector::outputYAttr;
MObject SmoothstepVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SmoothstepVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SmoothstepVector::initialize()
{
	MVector vector;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(tAttr, "t", "t", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tAttr, 0.0, 1.0);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(tAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(tAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SmoothstepVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);
	double t = inputDoubleValue(dataBlock, tAttr);
	t = t * t * (3 - 2 * t);

	outputVectorValue(dataBlock, outputAttr, input1 + (input2 - input1) * t);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------