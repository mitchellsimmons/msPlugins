#include "arithmeticSequenceDoubleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArithmeticSequenceDoubleArray::ArithmeticSequenceDoubleArray() {}
ArithmeticSequenceDoubleArray::~ArithmeticSequenceDoubleArray() {}

// ------ Attr ------
MObject ArithmeticSequenceDoubleArray::firstTermAttr;
MObject ArithmeticSequenceDoubleArray::commonDifferenceAttr;
MObject ArithmeticSequenceDoubleArray::sizeAttr;
MObject ArithmeticSequenceDoubleArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ArithmeticSequenceDoubleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ArithmeticSequenceDoubleArray::initialize()
{
	std::vector<double> outputs;

	createDoubleAttribute(firstTermAttr, "firstTerm", "firstTerm", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(commonDifferenceAttr, "commonDifference", "commonDifference", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createDoubleDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonDifferenceAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonDifferenceAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus ArithmeticSequenceDoubleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	double a = inputDoubleValue(dataBlock, firstTermAttr);
	double d = inputDoubleValue(dataBlock, commonDifferenceAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] + d;

	outputDoubleDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------