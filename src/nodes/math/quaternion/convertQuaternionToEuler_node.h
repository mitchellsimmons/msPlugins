#pragma once

#include <unordered_map>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MQuaternion.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class ConvertQuaternionToEuler : public MPxNode, MRS::NodeHelper
{
public:
	ConvertQuaternionToEuler();
	~ConvertQuaternionToEuler();

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
	static MObject inputWAttr;
	static MObject rotationOrderAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------