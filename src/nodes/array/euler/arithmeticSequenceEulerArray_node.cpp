#include "arithmeticSequenceEulerArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArithmeticSequenceEulerArray::ArithmeticSequenceEulerArray() {}
ArithmeticSequenceEulerArray::~ArithmeticSequenceEulerArray() {}

// ------ Attr ------
MObject ArithmeticSequenceEulerArray::firstTermAttr;
MObject ArithmeticSequenceEulerArray::firstTermXAttr;
MObject ArithmeticSequenceEulerArray::firstTermYAttr;
MObject ArithmeticSequenceEulerArray::firstTermZAttr;
MObject ArithmeticSequenceEulerArray::commonDifferenceAttr;
MObject ArithmeticSequenceEulerArray::commonDifferenceXAttr;
MObject ArithmeticSequenceEulerArray::commonDifferenceYAttr;
MObject ArithmeticSequenceEulerArray::commonDifferenceZAttr;
MObject ArithmeticSequenceEulerArray::sizeAttr;
MObject ArithmeticSequenceEulerArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ArithmeticSequenceEulerArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ArithmeticSequenceEulerArray::initialize()
{
	std::vector<MEulerRotation> outputs;
	MEulerRotation firstTerm;
	MEulerRotation commonDifference;

	createEulerAttribute(firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr, 
		"firstTerm", "firstTerm", firstTerm, kDefaultPreset | kKeyable);
	createEulerAttribute(commonDifferenceAttr, commonDifferenceXAttr, commonDifferenceYAttr, commonDifferenceZAttr, 
		"commonDifference", "commonDifference", commonDifference, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonDifferenceAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonDifferenceAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus ArithmeticSequenceEulerArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MEulerRotation a = inputEulerValue(dataBlock, firstTermAttr, firstTermXAttr, firstTermYAttr, firstTermZAttr);
	MEulerRotation d = inputEulerValue(dataBlock, commonDifferenceAttr, commonDifferenceXAttr, commonDifferenceYAttr, commonDifferenceZAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);
	
	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1] + d;

	outputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------