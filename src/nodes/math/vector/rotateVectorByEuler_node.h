#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class RotateVectorByEuler : public MPxNode, MRS::NodeHelper
{
public:
	RotateVectorByEuler();
	~RotateVectorByEuler();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject inputAttr;
	static MObject inputXAttr;
	static MObject inputYAttr;
	static MObject inputZAttr;
	static MObject rotationAttr;
	static MObject rotationXAttr;
	static MObject rotationYAttr;
	static MObject rotationZAttr;
	static MObject rotationOrderAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------