#pragma once

#include <algorithm>

#include <maya/MArrayDataHandle.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/node_utils.h"
#include "data/eulerArray_data.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class PackEulerArray : public MPxNode, MRS::NodeHelper
{
public:
	PackEulerArray();
	~PackEulerArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputRotateAttr;
	static MObject inputRotateXAttr;
	static MObject inputRotateYAttr;
	static MObject inputRotateZAttr;
	static MObject fillRotateAttr;
	static MObject fillRotateXAttr;
	static MObject fillRotateYAttr;
	static MObject fillRotateZAttr;
	static MObject fillLogicallyAttr;
	static MObject sizeAttr;
	static MObject outputRotateAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MEulerRotation> m_rotations;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------