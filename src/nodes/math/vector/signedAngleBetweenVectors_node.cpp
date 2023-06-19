#include "signedAngleBetweenVectors_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

SignedAngleBetweenVectors::SignedAngleBetweenVectors() {}
SignedAngleBetweenVectors::~SignedAngleBetweenVectors() {}

// ------ Attr ------
MObject SignedAngleBetweenVectors::unitAxisAttr;
MObject SignedAngleBetweenVectors::unitAxisXAttr;
MObject SignedAngleBetweenVectors::unitAxisYAttr;
MObject SignedAngleBetweenVectors::unitAxisZAttr;
MObject SignedAngleBetweenVectors::input1Attr;
MObject SignedAngleBetweenVectors::input1XAttr;
MObject SignedAngleBetweenVectors::input1YAttr;
MObject SignedAngleBetweenVectors::input1ZAttr;
MObject SignedAngleBetweenVectors::input2Attr;
MObject SignedAngleBetweenVectors::input2XAttr;
MObject SignedAngleBetweenVectors::input2YAttr;
MObject SignedAngleBetweenVectors::input2ZAttr;
MObject SignedAngleBetweenVectors::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType SignedAngleBetweenVectors::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus SignedAngleBetweenVectors::initialize()
{
	createVectorAttribute(input1Attr, input1XAttr, input1YAttr, input1ZAttr, "input1", "input1", MVector::zero, kDefaultPreset | kKeyable);
	createVectorAttribute(input2Attr, input2XAttr, input2YAttr, input2ZAttr, "input2", "input2", MVector::zero, kDefaultPreset | kKeyable);
	createVectorAttribute(unitAxisAttr, unitAxisXAttr, unitAxisYAttr, unitAxisZAttr, "unitAxis", "unitAxis", MVector::yAxis, kDefaultPreset | kKeyable);
	createAngleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

	addAttribute(input1Attr);
	addAttribute(input2Attr);
	addAttribute(unitAxisAttr);
	addAttribute(outputAttr);

	attributeAffects(input1Attr, outputAttr);
	attributeAffects(input2Attr, outputAttr);
	attributeAffects(unitAxisAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus SignedAngleBetweenVectors::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input1 = inputVectorValue(dataBlock, input1Attr);
	MVector input2 = inputVectorValue(dataBlock, input2Attr);
	MVector axis = inputVectorValue(dataBlock, unitAxisAttr);

	outputAngleValue(dataBlock, outputAttr, std::atan2((input1 ^ input2) * axis, input1 * input2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------