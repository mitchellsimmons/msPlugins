#include "divideVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DivideVector::DivideVector() {}
DivideVector::~DivideVector() {}

// ------ Attr ------
MObject DivideVector::input1Attr;
MObject DivideVector::input1XAttr;
MObject DivideVector::input1YAttr;
MObject DivideVector::input1ZAttr;
MObject DivideVector::input2Attr;
MObject DivideVector::outputAttr;
MObject DivideVector::outputXAttr;
MObject DivideVector::outputYAttr;
MObject DivideVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DivideVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DivideVector::initialize()
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

MStatus DivideVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	double input2 = inputDoubleValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 / input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------