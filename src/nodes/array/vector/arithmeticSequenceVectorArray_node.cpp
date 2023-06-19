#include "arithmeticSequenceVectorArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArithmeticSequenceVectorArray::ArithmeticSequenceVectorArray() {}
ArithmeticSequenceVectorArray::~ArithmeticSequenceVectorArray() {}

// ------ Attr ------
MObject ArithmeticSequenceVectorArray::firstTermAttr;
MObject ArithmeticSequenceVectorArray::firstTermXAttr;
MObject ArithmeticSequenceVectorArray::firstTermYAttr;
MObject ArithmeticSequenceVectorArray::firstTermZAttr;
MObject ArithmeticSequenceVectorArray::commonDifferenceAttr;
MObject ArithmeticSequenceVectorArray::commonDifferenceXAttr;
MObject ArithmeticSequenceVectorArray::commonDifferenceYAttr;
MObject ArithmeticSequenceVectorArray::commonDifferenceZAttr;
MObject ArithmeticSequenceVectorArray::sizeAttr;
MObject ArithmeticSequenceVectorArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ArithmeticSequenceVectorArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ArithmeticSequenceVectorArray::initialize()
{
	std::vector<MVector> outputs;
	MVector firstTerm;
	MVector commonDifference;

	createVectorAttribute(firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr,
		"firstTerm", "firstTerm", firstTerm, kDefaultPreset | kKeyable);
	createVectorAttribute(commonDifferenceAttr, commonDifferenceXAttr, commonDifferenceYAttr, commonDifferenceZAttr,
		"commonDifference", "commonDifference", commonDifference, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createVectorDataArrayAttribute(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonDifferenceAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonDifferenceAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus ArithmeticSequenceVectorArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MVector a = inputVectorValue(dataBlock, firstTermAttr);
	MVector d = inputVectorValue(dataBlock, commonDifferenceAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] + d;

	outputVectorDataArrayValue(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------