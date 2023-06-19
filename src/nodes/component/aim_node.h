#pragma once

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

class Aim : public MPxNode, MRS::NodeHelper
{
public:
	Aim();
	~Aim();

	// ------ Const ------
	enum Axis : short
	{
		kPosX = 0,
		kPosY = 1,
		kPosZ = 2,
		kNegX = 3,
		kNegY = 4,
		kNegZ = 5,
	};

	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject rootPositionAttr;
	static MObject aimPositionAttr;
	static MObject upPositionAttr;
	static MObject aimAxisAttr;
	static MObject upAxisAttr;
	static MObject outputAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------