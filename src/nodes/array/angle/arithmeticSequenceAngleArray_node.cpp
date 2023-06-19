#include "arithmeticSequenceAngleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ArithmeticSequenceAngleArray::ArithmeticSequenceAngleArray() {}
ArithmeticSequenceAngleArray::~ArithmeticSequenceAngleArray() {}

// ------ Attr ------
MObject ArithmeticSequenceAngleArray::firstTermAttr;
MObject ArithmeticSequenceAngleArray::commonDifferenceAttr;
MObject ArithmeticSequenceAngleArray::sizeAttr;
MObject ArithmeticSequenceAngleArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ArithmeticSequenceAngleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ArithmeticSequenceAngleArray::initialize()
{
	std::vector<MAngle> outputs;

	createAngleAttribute(firstTermAttr, "firstTerm", "firstTerm", 0.0, kDefaultPreset | kKeyable);
	createAngleAttribute(commonDifferenceAttr, "commonDifference", "commonDifference", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<AngleArrayData, MAngle>(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonDifferenceAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonDifferenceAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus ArithmeticSequenceAngleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MAngle a = inputAngleValue(dataBlock, firstTermAttr);
	MAngle d = inputAngleValue(dataBlock, commonDifferenceAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1].asRadians() + d.asRadians();

	outputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------