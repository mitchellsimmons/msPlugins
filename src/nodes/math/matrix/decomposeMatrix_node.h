#pragma once

#include <unordered_map>

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
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

//	Performance
//	-----------
//	All outputs connected
//	Parallel Evaluation: 13-15% improvement over default decomposeMatrix node

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class DecomposeMatrix : public MPxNode, MRS::NodeHelper
{
public:
	DecomposeMatrix();
	~DecomposeMatrix();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputAttr;
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
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------