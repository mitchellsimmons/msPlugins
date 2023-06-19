#include "averageEuler_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

AverageEuler::AverageEuler() {}
AverageEuler::~AverageEuler() {}

// ------ Attr ------
MObject AverageEuler::inputRotationOrderAttr;
MObject AverageEuler::inputAttr;
MObject AverageEuler::outputRotationOrderAttr;
MObject AverageEuler::outputAttr;
MObject AverageEuler::outputXAttr;
MObject AverageEuler::outputYAttr;
MObject AverageEuler::outputZAttr;

// ------ MPxNode ------
MPxNode::SchedulingType AverageEuler::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus AverageEuler::initialize()
{
	std::vector<int> inputRotationOrder;
	std::unordered_map<const char*, short> outputRotationOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	std::vector<MEulerRotation> inputs;
	MEulerRotation output;

	createIntDataArrayAttribute(inputRotationOrderAttr, "inputRotationOrder", "inputRotationOrder", inputRotationOrder, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(inputAttr, "input", "input", inputs, kDefaultPreset | kKeyable);
	createEnumAttribute(outputRotationOrderAttr, "outputRotationOrder", "outputRotationOrder", outputRotationOrderFields, 0, kDefaultPreset | kKeyable);
	createEulerAttribute(outputAttr, outputXAttr, outputYAttr, outputZAttr, "output", "output", output, kReadOnlyPreset);

	addAttribute(inputRotationOrderAttr);
	addAttribute(inputAttr);
	addAttribute(outputRotationOrderAttr);
	addAttribute(outputAttr);

	attributeAffects(inputRotationOrderAttr, outputAttr);
	attributeAffects(inputAttr, outputAttr);
	attributeAffects(outputRotationOrderAttr, outputAttr);

	return MStatus::kSuccess;
}

MStatus AverageEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputRotationOrder = inputIntDataArrayValue(dataBlock, inputRotationOrderAttr);
	std::vector<MEulerRotation> inputs = inputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, inputAttr);
	short outputRotationOrder = inputEnumValue(dataBlock, outputRotationOrderAttr);
	size_t count = inputs.size();
	inputRotationOrder.resize(count, 0);

	MEulerRotation average = MEulerRotation::identity;
	if (count)
	{
		m_rotations.resize(count);
		for (unsigned int i = 0; i < count; ++i)
		{
			MEulerRotation& euler = inputs[i];
			euler.order = (MEulerRotation::RotationOrder)MRS::clamp(inputRotationOrder[i], 0, 5);
			m_rotations[i] = euler.asQuaternion();
		}

		MQuaternion qAverage = MRS::averageQuaternion(m_rotations);
		average = qAverage.asEulerRotation();
		average.reorderIt((MEulerRotation::RotationOrder)outputRotationOrder);
	}
	
	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, average);

	return MStatus::kSuccess;
}

// Initial attempt, averaging weighted logarithms, based on following thread
// https://forum.unity.com/threads/average-quaternions.86898/
// Formulas for logarithm/exponential
// https://en.wikipedia.org/wiki/Quaternion#Exponential,_logarithm,_and_power_functions

// A rotation represented by an axis and an angle can be represented in a complex exponential form, e^(axis/|axis| * angle / 2)
// Each component of the axis acts as a scalar for an imaginary basis vector
// Exponentials of this form can be converted to the standard quaternion form as long as the axis is normal
// To determine the exponent of a single rotation we need to determine the axis and angle from the euler rotation
// Or we can use the MQuaternion::log() function (returns a pure quaternion, ie. w=0, v=exponent)

/*MStatus AverageEuler::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputXAttr && plug != outputYAttr && plug != outputZAttr && plug != outputAttr)
		return MStatus::kUnknownParameter;

	std::vector<int> inputRotationOrder = inputIntDataArrayValue(dataBlock, inputRotationOrderAttr);
	std::vector<MEulerRotation> inputs = inputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, inputAttr);
	short outputRotationOrder = inputEnumValue(dataBlock, outputRotationOrderAttr);
	unsigned int size = (int)inputs.size();
	inputRotationOrder.resize(size, 0);

	MVector vExponent;
	for (unsigned int i = 0; i < size; ++i)
	{
		MEulerRotation& euler = inputs[i];
		euler.order = (MEulerRotation::RotationOrder)MRS::clamp(inputRotationOrder[i], 0, 5);
		MQuaternion qRot = euler.asQuaternion();
		// The following is equivalent to calling qRot.log()
		MVector vAxis;
		double angle;
		qRot.getAxisAngle(vAxis, angle);
		vExponent += vAxis * angle * 0.5;
	}
	vExponent /= (double)inputs.size();

	// Exponentiate the average logarithm into standard quaternion form (length of the exponent vector represents the angle)
	// This is equivalent to calling the exp() function on the pure quaternion representation of the exponent vector
	MVector vAverage = sin(vExponent.length()) * vExponent.normal();
	MQuaternion qAverage{ vAverage.x, vAverage.y, vAverage.z, cos(vExponent.length()) };
	MEulerRotation average = qAverage.asEulerRotation();
	average.reorderIt((MEulerRotation::RotationOrder)outputRotationOrder);

	outputEulerValue(dataBlock, outputAttr, outputXAttr, outputYAttr, outputZAttr, average);

	return MStatus::kSuccess;
}*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------