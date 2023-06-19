#include "addVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AddVector::AddVector() {}
AddVector::~AddVector() {}

// ------ Attr ------
MObject AddVector::input1Attr;
MObject AddVector::input1XAttr;
MObject AddVector::input1YAttr;
MObject AddVector::input1ZAttr;
MObject AddVector::input2Attr;
MObject AddVector::input2XAttr;
MObject AddVector::input2YAttr;
MObject AddVector::input2ZAttr;
MObject AddVector::outputAttr;
MObject AddVector::outputXAttr;
MObject AddVector::outputYAttr;
MObject AddVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AddVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AddVector::initialize()
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

MStatus AddVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);

	outputVectorValue(dataBlock, outputAttr, input1 + input2);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------