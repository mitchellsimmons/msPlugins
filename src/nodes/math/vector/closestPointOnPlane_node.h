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

#include "utils/math_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ClosestPointOnPlane : public MPxNode, MRS::NodeHelper
{
public:
	ClosestPointOnPlane();
	~ClosestPointOnPlane();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject targetAttr;
	static MObject targetXAttr;
	static MObject targetYAttr;
	static MObject targetZAttr;
	static MObject pointOnPlaneAttr;
	static MObject pointOnPlaneXAttr;
	static MObject pointOnPlaneYAttr;
	static MObject pointOnPlaneZAttr;
	static MObject normalToPlaneAttr;
	static MObject normalToPlaneXAttr;
	static MObject normalToPlaneYAttr;
	static MObject normalToPlaneZAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------