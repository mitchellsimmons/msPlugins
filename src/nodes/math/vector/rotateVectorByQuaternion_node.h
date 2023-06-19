#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/node_utils.h"
#include "utils/vector_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class RotateVectorByQuaternion : public MPxNode, MRS::NodeHelper
{
public:
	RotateVectorByQuaternion();
	~RotateVectorByQuaternion();

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
	static MObject unitQuaternionAttr;
	static MObject unitQuaternionXAttr;
	static MObject unitQuaternionYAttr;
	static MObject unitQuaternionZAttr;
	static MObject unitQuaternionWAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------