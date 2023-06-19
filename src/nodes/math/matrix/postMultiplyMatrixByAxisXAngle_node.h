#pragma once

#include <maya/MAngle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PostMultiplyMatrixByAxisXAngle : public MPxNode, MRS::NodeHelper
{
public:
	PostMultiplyMatrixByAxisXAngle();
	~PostMultiplyMatrixByAxisXAngle();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputAttr;
	static MObject angleXAttr;
	static MObject outputAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------