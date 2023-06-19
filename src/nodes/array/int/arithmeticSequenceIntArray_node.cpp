#include "arithmeticSequenceIntArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArithmeticSequenceIntArray::ArithmeticSequenceIntArray() {}
ArithmeticSequenceIntArray::~ArithmeticSequenceIntArray() {}

// ------ Attr ------
MObject ArithmeticSequenceIntArray::firstTermAttr;
MObject ArithmeticSequenceIntArray::commonDifferenceAttr;
MObject ArithmeticSequenceIntArray::sizeAttr;
MObject ArithmeticSequenceIntArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ArithmeticSequenceIntArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ArithmeticSequenceIntArray::initialize()
{
	std::vector<int> outputs;

	createIntAttribute(firstTermAttr, "firstTerm", "firstTerm", 0, kDefaultPreset | kKeyable);
	createIntAttribute(commonDifferenceAttr, "commonDifference", "commonDifference", 0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createIntDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonDifferenceAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonDifferenceAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus ArithmeticSequenceIntArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	int a = inputIntValue(dataBlock, firstTermAttr);
	int d = inputIntValue(dataBlock, commonDifferenceAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] + d;

	outputIntDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------