#include "remap_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

Remap::Remap() {}
Remap::~Remap() {}

// ------ Attr ------
MObject Remap::inputAttr;
MObject Remap::low1Attr;
MObject Remap::high1Attr;
MObject Remap::low2Attr;
MObject Remap::high2Attr;
MObject Remap::outputAttr;

// ------ MPxNode ------
MPxNode::SchedulingType Remap::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus Remap::initialize()
{
	createDoubleAttribute(inputAttr, "input", "input", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(low1Attr, "low1", "low1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(high1Attr, "high1", "high1", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(low2Attr, "low2", "low2", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(high2Attr, "high2", "high2", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(outputAttr, "output", "output", 0.0, kReadOnlyPreset);

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

MStatus Remap::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAttr)
		return MStatus::kUnknownParameter;

	double input = inputDoubleValue(dataBlock, inputAttr);
	double low1 = inputDoubleValue(dataBlock, low1Attr);
	double high1 = inputDoubleValue(dataBlock, high1Attr);
	double low2 = inputDoubleValue(dataBlock, low2Attr);
	double high2 = inputDoubleValue(dataBlock, high2Attr);

	outputDoubleValue(dataBlock, outputAttr, MRS::remap(input, low1, high1, low2, high2));

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------