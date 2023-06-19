#include "geometricSequenceAngleArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

GeometricSequenceAngleArray::GeometricSequenceAngleArray() {}
GeometricSequenceAngleArray::~GeometricSequenceAngleArray() {}

// ------ Attr ------
MObject GeometricSequenceAngleArray::firstTermAttr;
MObject GeometricSequenceAngleArray::commonRatioAttr;
MObject GeometricSequenceAngleArray::sizeAttr;
MObject GeometricSequenceAngleArray::outputSequenceAttr;

// ------ MPxNode ------
MPxNode::SchedulingType GeometricSequenceAngleArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus GeometricSequenceAngleArray::initialize()
{
	std::vector<MAngle> outputs;

	createAngleAttribute(firstTermAttr, "firstTerm", "firstTerm", 0.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(commonRatioAttr, "commonRatio", "commonRatio", 0.0, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createPluginDataArrayAttribute<AngleArrayData, MAngle>(outputSequenceAttr, "outputSequence", "outputSequence", outputs, kReadOnlyPreset);

	addAttribute(firstTermAttr);
	addAttribute(commonRatioAttr);
	addAttribute(sizeAttr);
	addAttribute(outputSequenceAttr);

	attributeAffects(firstTermAttr, outputSequenceAttr);
	attributeAffects(commonRatioAttr, outputSequenceAttr);
	attributeAffects(sizeAttr, outputSequenceAttr);

	return MStatus::kSuccess;
}

MStatus GeometricSequenceAngleArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputSequenceAttr)
		return MStatus::kUnknownParameter;

	MAngle a = inputAngleValue(dataBlock, firstTermAttr);
	MAngle r = inputDoubleValue(dataBlock, commonRatioAttr);
	unsigned int size = inputIntValue(dataBlock, sizeAttr);

	// Recursively compute the sequence
	m_outputSequence.resize(size);
	m_outputSequence[0] = a;
	for (unsigned int n = 1; n < size; ++n)
		m_outputSequence[n] = m_outputSequence[n - 1].asRadians() * r.asRadians();

	outputPluginDataArrayValue<AngleArrayData, MAngle>(dataBlock, outputSequenceAttr, m_outputSequence);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------