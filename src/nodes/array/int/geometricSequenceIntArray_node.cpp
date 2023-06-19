#include "geometricSequenceIntArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GeometricSequenceIntArray::GeometricSequenceIntArray() {}
GeometricSequenceIntArray::~GeometricSequenceIntArray() {}

// ------ Attr ------
MObject GeometricSequenceIntArray::firstTermAttr;
MObject GeometricSequenceIntArray::commonRatioAttr;
MObject GeometricSequenceIntArray::sizeAttr;
MObject GeometricSequenceIntArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType GeometricSequenceIntArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus GeometricSequenceIntArray::initialize()
{
	std::vector<int> outputs;

	createIntAttribute(firstTermAttr, "firstTerm", "firstTerm", 0, kDefaultPreset | kKeyable);
	createIntAttribute(commonRatioAttr, "commonRatio", "commonRatio", 0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createIntDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonRatioAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonRatioAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus GeometricSequenceIntArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	int a = inputIntValue(dataBlock, firstTermAttr);
	int r = inputIntValue(dataBlock, commonRatioAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] * r;

	outputIntDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------