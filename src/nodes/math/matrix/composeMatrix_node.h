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

class ComposeMatrix : public MPxNode, MRS::NodeHelper
{
public:
	ComposeMatrix();
	~ComposeMatrix();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject translationAttr;
	static MObject translationXAttr;
	static MObject translationYAttr;
	static MObject translationZAttr;
	static MObject rotationAttr;
	static MObject rotationXAttr;
	static MObject rotationYAttr;
	static MObject rotationZAttr;
	static MObject scaleAttr;
	static MObject scaleXAttr;
	static MObject scaleYAttr;
	static MObject scaleZAttr;
	static MObject quaternionAttr;
	static MObject quaternionXAttr;
	static MObject quaternionYAttr;
	static MObject quaternionZAttr;
	static MObject quaternionWAttr;
	static MObject rotationOrderAttr;
	static MObject useEulerRotationAttr;
	static MObject outputAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------