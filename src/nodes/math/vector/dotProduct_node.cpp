#include "dotProduct_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DotProduct::DotProduct() {}
DotProduct::~DotProduct() {}

// ------ Attr ------
MObject DotProduct::input1Attr;
MObject DotProduct::input1XAttr;
MObject DotProduct::input1YAttr;
MObject DotProduct::input1ZAttr;
MObject DotProduct::input2Attr;
MObject DotProduct::input2XAttr;
MObject DotProduct::input2YAttr;
MObject DotProduct::input2ZAttr;
MObject DotProduct::outputAttr;
MObject DotProduct::outputXAttr;
MObject DotProduct::outputYAttr;
MObject DotProduct::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DotProduct::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DotProduct::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus DotProduct::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);

	outputDoubleValue(dataBlock, outputAttr, input1 * input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------