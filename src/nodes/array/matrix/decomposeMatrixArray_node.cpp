#include "decomposeMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

DecomposeMatrixArray::DecomposeMatrixArray() : MPxNode(), NodeHelper() {}
DecomposeMatrixArray::~DecomposeMatrixArray() {}

// ------ Attr ------
MObject DecomposeMatrixArray::inputMatrixAttr;
MObject DecomposeMatrixArray::rotationOrderAttr;
MObject DecomposeMatrixArray::outputTranslationAttr;
MObject DecomposeMatrixArray::outputTranslationXAttr;
MObject DecomposeMatrixArray::outputTranslationYAttr;
MObject DecomposeMatrixArray::outputTranslationZAttr;
MObject DecomposeMatrixArray::outputRotationAttr;
MObject DecomposeMatrixArray::outputRotationXAttr;
MObject DecomposeMatrixArray::outputRotationYAttr;
MObject DecomposeMatrixArray::outputRotationZAttr;
MObject DecomposeMatrixArray::outputScaleAttr;
MObject DecomposeMatrixArray::outputScaleXAttr;
MObject DecomposeMatrixArray::outputScaleYAttr;
MObject DecomposeMatrixArray::outputScaleZAttr;
MObject DecomposeMatrixArray::outputQuaternionAttr;
MObject DecomposeMatrixArray::outputQuaternionXAttr;
MObject DecomposeMatrixArray::outputQuaternionYAttr;
MObject DecomposeMatrixArray::outputQuaternionZAttr;
MObject DecomposeMatrixArray::outputQuaternionWAttr;

// ------ MPxNode ------
MPxNode::SchedulingType DecomposeMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus DecomposeMatrixArray::initialize()
{
	std::vector<MMatrix> input;
	std::vector<int> rotationOrder;
	MVector outputTranslation;
	MEulerRotation outputRotation;
	MVector outputScale;
	MQuaternion outputQuaternion;

	createMatrixDataArrayAttribute(inputMatrixAttr, "input", "input", input, kDefaultPreset | kKeyable);
	createIntDataArrayAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrder, kDefaultPreset | kKeyable);
	createVectorAttribute(outputTranslationAttr, outputTranslationXAttr, outputTranslationYAttr, outputTranslationZAttr, 
		"outputTranslation", "outputTranslation", outputTranslation, kReadOnlyArrayPreset);
	createEulerAttribute(outputRotationAttr, outputRotationXAttr, outputRotationYAttr, outputRotationZAttr, 
		"outputRotation", "outputRotation", outputRotation, kReadOnlyArrayPreset);
	createVectorAttribute(outputScaleAttr, outputScaleXAttr, outputScaleYAttr, outputScaleZAttr, 
		"outputScale", "outputScale", outputScale, kReadOnlyArrayPreset);
	createQuaternionAttribute(outputQuaternionAttr, outputQuaternionXAttr, outputQuaternionYAttr, outputQuaternionZAttr, outputQuaternionWAttr,
		"outputQuaternion", "outputQuaternion", outputQuaternion, kReadOnlyArrayPreset);

	addAttribute(inputMatrixAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(outputTranslationAttr);
	addAttribute(outputRotationAttr);
	addAttribute(outputScaleAttr);
	addAttribute(outputQuaternionAttr);

	attributeAffects(inputMatrixAttr, outputTranslationAttr);
	attributeAffects(inputMatrixAttr, outputRotationAttr);
	attributeAffects(rotationOrderAttr, outputRotationAttr);
	attributeAffects(inputMatrixAttr, outputScaleAttr);
	attributeAffects(inputMatrixAttr, outputQuaternionAttr);

	return MStatus::kSuccess;
}

MStatus DecomposeMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug == outputTranslationXAttr || plug == outputTranslationYAttr || plug == outputTranslationZAttr || plug == outputTranslationAttr)
	{
		std::vector<MMatrix> transforms = inputMatrixDataArrayValue(dataBlock, inputMatrixAttr);
		size_t size = transforms.size();
		m_translation.resize(size);
		for (unsigned int i = 0; i < size; ++i)
			m_translation[i] = MRS::extractTranslation(transforms[i]);

		outputVectorArrayValue(dataBlock, outputTranslationAttr, m_translation);
	}
	else if (plug == outputRotationXAttr || plug == outputRotationYAttr || plug == outputRotationZAttr || plug == outputRotationAttr)
	{
		std::vector<MMatrix> transforms = inputMatrixDataArrayValue(dataBlock, inputMatrixAttr);
		size_t size = transforms.size();
		std::vector<int> rotationOrder = inputIntDataArrayValue(dataBlock, rotationOrderAttr);
		rotationOrder.resize(size, 0);
		m_euler.resize(size);
		for (unsigned int i = 0; i < size; ++i)
			m_euler[i] = MRS::extractEulerRotation(transforms[i], (MEulerRotation::RotationOrder)MRS::clamp(rotationOrder[i], 0, 5));

		outputEulerArrayValue(dataBlock, outputRotationAttr, outputRotationXAttr, outputRotationYAttr, outputRotationZAttr, m_euler);
	}
	else if (plug == outputScaleXAttr || plug == outputScaleYAttr || plug == outputScaleZAttr || plug == outputScaleAttr)
	{
		std::vector<MMatrix> transforms = inputMatrixDataArrayValue(dataBlock, inputMatrixAttr);
		size_t size = transforms.size();
		m_scale.resize(size);
		for (unsigned int i = 0; i < size; ++i)
			m_scale[i] = MRS::extractScale(transforms[i]);

		outputVectorArrayValue(dataBlock, outputScaleAttr, m_scale);
	}
	else if (plug == outputQuaternionXAttr || plug == outputQuaternionYAttr || plug == outputQuaternionZAttr || plug == outputQuaternionWAttr || plug == outputQuaternionAttr)
	{
		std::vector<MMatrix> transforms = inputMatrixDataArrayValue(dataBlock, inputMatrixAttr);
		size_t size = transforms.size();
		m_quaternion.resize(size);
		for (unsigned int i = 0; i < size; ++i)
			m_quaternion[i] = MRS::extractQuaternionRotation(transforms[i]);

		outputQuaternionArrayValue(dataBlock, outputQuaternionAttr, outputQuaternionXAttr, 
			outputQuaternionYAttr, outputQuaternionZAttr, outputQuaternionWAttr, m_quaternion);
	}
	else
		return MStatus::kUnknownParameter;

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------