#include "remapInt_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

RemapInt::RemapInt() {}
RemapInt::~RemapInt() {}

// ------ Attr ------
MObject RemapInt::inputAttr;
MObject RemapInt::low1Attr;
MObject RemapInt::high1Attr;
MObject RemapInt::low2Attr;
MObject RemapInt::high2Attr;
MObject RemapInt::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType RemapInt::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus RemapInt::initialize()
{
	createIntAttribute(inputAttr, "input", "input", 0, kDefaultPreset | kKeyable);
	createIntAttribute(low1Attr, "low1", "low1", 0, kDefaultPreset | kKeyable);
	createIntAttribute(high1Attr, "high1", "high1", 0, kDefaultPreset | kKeyable);
	createIntAttribute(low2Attr, "low2", "low2", 0, kDefaultPreset | kKeyable);
	createIntAttribute(high2Attr, "high2", "high2", 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputAttr, "output", "output", 0, kReadOnlyPreset);

	addAttribute(inputAttr);
	addAttribute(low1Attr);
	addAttribute(high1Attr);
	addAttribute(low2Attr);
	addAttribute(high2Attr);
	addAttribute(outputAttr);

	attributeAffects(inputAttr, outputAttr);
	attributeAffects(low1Attr, outputAttr);
	attributeAffects(high1Attr, outputAttr);
	attributeAffects(low2Attr, outputAttr);
	attributeAffects(high2Attr, outputAttr);

	return MStatus::kSuccess;
}

MStatus RemapInt::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	int input = inputIntValue(dataBlock, inputAttr);
	int low1 = inputIntValue(dataBlock, low1Attr);
	int high1 = inputIntValue(dataBlock, high1Attr);
	int low2 = inputIntValue(dataBlock, low2Attr);
	int high2 = inputIntValue(dataBlock, high2Attr);

	outputIntValue(dataBlock, outputAttr, MRS::remap(input, low1, high1, low2, high2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------