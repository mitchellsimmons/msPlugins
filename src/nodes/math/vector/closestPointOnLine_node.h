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

class ClosestPointOnLine : public MPxNode, MRS::NodeHelper
{
public:
	ClosestPointOnLine();
	~ClosestPointOnLine();

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
	static MObject pointOnLine1Attr;
	static MObject pointOnLine1XAttr;
	static MObject pointOnLine1YAttr;
	static MObject pointOnLine1ZAttr;
	static MObject pointOnLine2Attr;
	static MObject pointOnLine2XAttr;
	static MObject pointOnLine2YAttr;
	static MObject pointOnLine2ZAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------