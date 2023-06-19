#include "composeMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

ComposeMatrixArray::ComposeMatrixArray() : MPxNode(), NodeHelper() {}
ComposeMatrixArray::~ComposeMatrixArray() {}

// ------ Attr ------
MObject ComposeMatrixArray::translationAttr;
MObject ComposeMatrixArray::rotationAttr;
MObject ComposeMatrixArray::scaleAttr;
MObject ComposeMatrixArray::quaternionAttr;
MObject ComposeMatrixArray::rotationOrderAttr;
MObject ComposeMatrixArray::useEulerRotationAttr;
MObject ComposeMatrixArray::sizeAttr;
MObject ComposeMatrixArray::outputMatrixAttr;

// ------ MPxNode ------
MPxNode::SchedulingType ComposeMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus ComposeMatrixArray::initialize()
{
	std::vector<MVector> translation;
	std::vector<MEulerRotation> rotation;
	std::vector<MVector> scale;
	std::vector<MQuaternion> quaternion;
	std::vector<int> rotationOrder;
	std::vector<MMatrix> out;

	createVectorDataArrayAttribute(translationAttr, "translation", "translation", translation, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(rotationAttr, "rotation", "rotation", rotation, kDefaultPreset | kKeyable);
	createVectorDataArrayAttribute(scaleAttr, "scale", "scale", scale, kDefaultPreset | kKeyable);
	createPluginDataArrayAttribute<QuaternionArrayData, MQuaternion>(quaternionAttr, "quaternion", "quaternion", quaternion, kDefaultPreset | kKeyable);
	createIntDataArrayAttribute(rotationOrderAttr, "rotationOrder", "rotationOrder", rotationOrder, kDefaultPreset | kKeyable);
	createBoolAttribute(useEulerRotationAttr, "useEulerRotation", "useEulerRotation", true, kDefaultPreset | kKeyable);
	createIntAttribute(sizeAttr, "size", "size", 0, kDefaultPreset | kKeyable);
	setMin(sizeAttr, 0);
	createMatrixDataArrayAttribute(outputMatrixAttr, "output", "output", out, kReadOnlyPreset);

	addAttribute(translationAttr);
	addAttribute(rotationAttr);
	addAttribute(scaleAttr);
	addAttribute(quaternionAttr);
	addAttribute(rotationOrderAttr);
	addAttribute(useEulerRotationAttr);
	addAttribute(sizeAttr);
	addAttribute(outputMatrixAttr);

	attributeAffects(translationAttr, outputMatrixAttr);
	attributeAffects(rotationAttr, outputMatrixAttr);
	attributeAffects(scaleAttr, outputMatrixAttr);
	attributeAffects(quaternionAttr, outputMatrixAttr);
	attributeAffects(rotationOrderAttr, outputMatrixAttr);
	attributeAffects(useEulerRotationAttr, outputMatrixAttr);
	attributeAffects(sizeAttr, outputMatrixAttr);

	return MStatus::kSuccess;
}

MStatus ComposeMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputMatrixAttr)
		return MStatus::kUnknownParameter;

	unsigned int size = inputIntValue(dataBlock, sizeAttr);
	bool useEuler = inputBoolValue(dataBlock, useEulerRotationAttr);
	std::vector<MVector> translation = inputVectorDataArrayValue(dataBlock, translationAttr);
	std::vector<MVector> scale = inputVectorDataArrayValue(dataBlock, scaleAttr);
	translation.resize(size);
	scale.resize(size, MVector::one);
	m_matrices.resize(size);

	if (useEuler)
	{
		std::vector<MEulerRotation> rotation = inputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, rotationAttr);
		std::vector<int> rotationOrder = inputIntDataArrayValue(dataBlock, rotationOrderAttr);
		rotation.resize(size);
		rotationOrder.resize(size, 0);

		for (unsigned int i = 0; i < size; ++i)
		{
			rotation[i].order = (MEulerRotation::RotationOrder)MRS::clamp(rotationOrder[i], 0, 5);
			m_matrices[i] = MRS::composeMatrix(translation[i], rotation[i], scale[i]);
		}
	}
	else
	{
		std::vector<MQuaternion> rotation = inputPluginDataArrayValue<QuaternionArrayData, MQuaternion>(dataBlock, quaternionAttr);
		rotation.resize(size);

		for (unsigned int i = 0; i < size; ++i)
			m_matrices[i] = MRS::composeMatrix(translation[i], rotation[i], scale[i]);
	}

	outputMatrixDataArrayValue(dataBlock, outputMatrixAttr, m_matrices);

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------