#include "unpackMatrixArray_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

UnpackMatrixArray::UnpackMatrixArray() : MPxNode(), NodeHelper() {}
UnpackMatrixArray::~UnpackMatrixArray() {}

// ------ Attr ------
MObject UnpackMatrixArray::inputMatrixAttr;
MObject UnpackMatrixArray::outputMatrixAttr;

// ------ MPxNode ------
MPxNode::SchedulingType UnpackMatrixArray::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus UnpackMatrixArray::initialize()
{
	std::vector<MMatrix> in;
	MMatrix out;

	createMatrixDataArrayAttribute(inputMatrixAttr, "input", "input", in, kDefaultPreset);
	createMatrixAttribute(outputMatrixAttr, "output", "output", out, kReadOnlyArrayPreset);

	addAttribute(inputMatrixAttr);
	addAttribute(outputMatrixAttr);

	attributeAffects(inputMatrixAttr, outputMatrixAttr);

	return MStatus::kSuccess;
}

MStatus UnpackMatrixArray::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputMatrixAttr)
		return MStatus::kUnknownParameter;

	MDataHandle inMatrixDataArrayHandle = dataBlock.inputValue(inputMatrixAttr);
	MArrayDataHandle outMatrixArrayHandle = dataBlock.outputArrayValue(outputMatrixAttr);

	MObject matrixDataObj = inMatrixDataArrayHandle.data();
	MFnMatrixArrayData fnMatrix(matrixDataObj);
	MArrayDataBuilder builder(&dataBlock, outputMatrixAttr, fnMatrix.length());

	for (unsigned int i = 0; i < fnMatrix.length(); i++)
	{
		MDataHandle outMatrixElementHandle = builder.addElement(i);
		outMatrixElementHandle.setMMatrix(fnMatrix[i]);
	}

	outMatrixArrayHandle.set(builder);
	outMatrixArrayHandle.setAllClean();

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------