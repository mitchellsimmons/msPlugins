#include "angleBetweenVectors_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AngleBetweenVectors::AngleBetweenVectors() {}
AngleBetweenVectors::~AngleBetweenVectors() {}

// ------ Attr ------
MObject AngleBetweenVectors::input1Attr;
MObject AngleBetweenVectors::input1XAttr;
MObject AngleBetweenVectors::input1YAttr;
MObject AngleBetweenVectors::input1ZAttr;
MObject AngleBetweenVectors::input2Attr;
MObject AngleBetweenVectors::input2XAttr;
MObject AngleBetweenVectors::input2YAttr;
MObject AngleBetweenVectors::input2ZAttr;
MObject AngleBetweenVectors::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AngleBetweenVectors::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AngleBetweenVectors::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", vector, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AngleBetweenVectors::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);

	outputAngleValue(dataBlock, outputAttr, input1.angle(input2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------