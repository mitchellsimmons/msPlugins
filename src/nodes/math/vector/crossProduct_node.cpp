#include "crossProduct_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

CrossProduct::CrossProduct() {}
CrossProduct::~CrossProduct() {}

// ------ Attr ------
MObject CrossProduct::input1Attr;
MObject CrossProduct::input1XAttr;
MObject CrossProduct::input1YAttr;
MObject CrossProduct::input1ZAttr;
MObject CrossProduct::input2Attr;
MObject CrossProduct::input2XAttr;
MObject CrossProduct::input2YAttr;
MObject CrossProduct::input2ZAttr;
MObject CrossProduct::outputAttr;
MObject CrossProduct::outputXAttr;
MObject CrossProduct::outputYAttr;
MObject CrossProduct::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType CrossProduct::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus CrossProduct::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus CrossProduct::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 ^ input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------