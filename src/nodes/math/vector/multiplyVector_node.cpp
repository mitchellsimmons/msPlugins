#include "multiplyVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MultiplyVector::MultiplyVector() {}
MultiplyVector::~MultiplyVector() {}

// ------ Attr ------
MObject MultiplyVector::input1Attr;
MObject MultiplyVector::input1XAttr;
MObject MultiplyVector::input1YAttr;
MObject MultiplyVector::input1ZAttr;
MObject MultiplyVector::input2Attr;
MObject MultiplyVector::outputAttr;
MObject MultiplyVector::outputXAttr;
MObject MultiplyVector::outputYAttr;
MObject MultiplyVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType MultiplyVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus MultiplyVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(input2Attr, "input2", "input2", 0.0, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus MultiplyVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------