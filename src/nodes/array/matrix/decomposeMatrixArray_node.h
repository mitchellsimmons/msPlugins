#pragma once

#include <algorithm>
#include <unordered_map>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "data/eulerArray_data.h"
#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DecomposeMatrixArray : public MPxNode, MRS::NodeHelper
{
public:
	DecomposeMatrixArray();
	~DecomposeMatrixArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputMatrixAttr;
	static MObject rotationOrderAttr;
	static MObject outputTranslationAttr;
	static MObject outputTranslationXAttr;
	static MObject outputTranslationYAttr;
	static MObject outputTranslationZAttr;
	static MObject outputRotationAttr;
	static MObject outputRotationXAttr;
	static MObject outputRotationYAttr;
	static MObject outputRotationZAttr;
	static MObject outputScaleAttr;
	static MObject outputScaleXAttr;
	static MObject outputScaleYAttr;
	static MObject outputScaleZAttr;
	static MObject outputQuaternionAttr;
	static MObject outputQuaternionXAttr;
	static MObject outputQuaternionYAttr;
	static MObject outputQuaternionZAttr;
	static MObject outputQuaternionWAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MVector> m_translation;
	std::vector<MEulerRotation> m_euler;
	std::vector<MVector> m_scale;
	std::vector<MQuaternion> m_quaternion;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------