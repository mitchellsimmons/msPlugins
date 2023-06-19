#pragma once

#include <maya/MAngle.h>
#include <maya/MArrayDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"
#include "data/eulerArray_data.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class UnpackEulerArray : public MPxNode, MRS::NodeHelper
{
public:
	UnpackEulerArray();
	~UnpackEulerArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ Attr ------
	static MObject inputRotateAttr;
	static MObject outputRotateAttr;
	static MObject outputRotateXAttr;
	static MObject outputRotateYAttr;
	static MObject outputRotateZAttr;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

private:
	// ------ Data ------
	std::vector<MEulerRotation> m_rotations;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------