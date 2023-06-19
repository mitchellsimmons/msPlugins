#include "negateVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

NegateVector::NegateVector() {}
NegateVector::~NegateVector() {}

// ------ Attr ------
MObject NegateVector::inputAttr;
MObject NegateVector::inputXAttr;
MObject NegateVector::inputYAttr;
MObject NegateVector::inputZAttr;
MObject NegateVector::outputAttr;
MObject NegateVector::outputXAttr;
MObject NegateVector::outputYAttr;
MObject NegateVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType NegateVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus NegateVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus NegateVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector v = inputVectorValue(dataBlock, inputAttr);

	outputVectorValue(dataBlock, outputAttr, v * -1);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------