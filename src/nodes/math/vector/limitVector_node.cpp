#include "limitVector_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

LimitVector::LimitVector() {}
LimitVector::~LimitVector() {}

// ------ Attr ------
MObject LimitVector::inputAttr;
MObject LimitVector::inputXAttr;
MObject LimitVector::inputYAttr;
MObject LimitVector::inputZAttr;
MObject LimitVector::minLengthAttr;
MObject LimitVector::maxLengthAttr;
MObject LimitVector::outputAttr;
MObject LimitVector::outputXAttr;
MObject LimitVector::outputYAttr;
MObject LimitVector::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType LimitVector::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus LimitVector::initialize()
{
	MVector vector = MVector::zero;

	createVectorAttribute(inputAttr, inputXAttr, inputYAttr, inputZAttr, "input", "input", vector, kDefaultPreset | kKeyable);
	createDoubleAttribute(minLengthAttr, "minLength", "minLength", 0.0, kDefaultPreset | kKeyable);
	setMin(minLengthAttr, 0.0);
	createDoubleAttribute(maxLengthAttr, "maxLength", "maxLength", 0.0, kDefaultPreset | kKeyable);
	setMin(maxLengthAttr, 0.0);
	createVectorAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", vector, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(minLengthAttr);
	addAttribute(maxLengthAttr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(minLengthAttr, outputAttr);
	attributeAffects(maxLengthAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus LimitVector::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	MVector input = inputVectorValue(dataBlock, inputAttr);
	double minLength = inputDoubleValue(dataBlock, minLengthAttr);
	double maxLength = inputDoubleValue(dataBlock, maxLengthAttr);

	double length = input.length();
	if (length < minLength && !MRS::isEqual(length, 0.0))
		input *= minLength / length;
	else if (length > maxLength)
		input *= maxLength / length;

	outputVectorValue(dataBlock, outputAttr, input);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------