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

// Node calculates the projection of a vector onto a plane defined by a normal vector
class VectorPlaneProjection : public MPxNode, MRS::NodeHelper
{
public:
	VectorPlaneProjection();
	~VectorPlaneProjection();

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