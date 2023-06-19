#include "lerpVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LerpVector::LerpVector() {}
LerpVector::~LerpVector() {}

// ------ Attr ------
MObject LerpVector::input1Attr;
MObject LerpVector::input1XAttr;
MObject LerpVector::input1YAttr;
MObject LerpVector::input1ZAttr;
MObject LerpVector::input2Attr;
MObject LerpVector::input2XAttr;
MObject LerpVector::input2YAttr;
MObject LerpVector::input2ZAttr;
MObject LerpVector::tAttr;
MObject LerpVector::outputAttr;
MObject LerpVector::outputXAttr;
MObject LerpVector::outputYAttr;
MObject LerpVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType LerpVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus LerpVector::initialize()
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

MStatus LerpVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);
	double t = inputDoubleValue(dataBlock, tAttr);

	outputVectorValue(dataBlock, outputAttr, input1 + (input2 - input1) * t);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------