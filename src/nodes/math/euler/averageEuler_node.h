#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MEulerRotation.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "data/eulerArray_data.h"
#include "utils/math_utils.h"
#include "utils/node_utils.h"
#include "utils/quaternion_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class AverageEuler : public MPxNode, MRS::NodeHelper
{
public:
	AverageEuler();
	~AverageEuler();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject inputRotationOrderAttr;
	static MObject inputAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
	static MObject outputRotationOrderAttr;

private:
	// ------ Data ------
	std::vector<MQuaternion> m_rotations;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------