#pragma once

#include <algorithm>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "data/eulerArray_data.h"
#include "data/quaternionArray_data.h"
#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ComposeMatrixArray : public MPxNode, MRS::NodeHelper
{
public:
	ComposeMatrixArray();
	~ComposeMatrixArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject translationAttr;
	static MObject rotationAttr;
	static MObject scaleAttr;
	static MObject quaternionAttr;
	static MObject rotationOrderAttr;
	static MObject useEulerRotationAttr;
	static MObject sizeAttr;
	static MObject outputMatrixAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MMatrix> m_matrices;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------