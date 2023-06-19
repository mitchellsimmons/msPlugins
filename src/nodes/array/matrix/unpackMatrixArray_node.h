#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnMatrixArrayData.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class UnpackMatrixArray : public MPxNode, MRS::NodeHelper
{
public:
	UnpackMatrixArray();
	~UnpackMatrixArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputMatrixAttr;
	static MObject outputMatrixAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;

	static MStatus initialize();

	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------