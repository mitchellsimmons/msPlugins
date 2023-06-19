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
#include <maya/MQuaternion.h>

#include "data/eulerArray_data.h"
#include "utils/math_utils.h"
#include "utils/node_utils.h"
#include "utils/quaternion_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class WeightedAverageEuler : public MPxNode, MRS::NodeHelper
{
public:
	WeightedAverageEuler();
	~WeightedAverageEuler();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject inputAttr;
	static MObject weightAttr;
	static MObject inputRotationOrderAttr;
	static MObject outputRotationOrderAttr;
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;

private:
	// ------ Data ------
	std::vector<MQuaternion> m_rotations;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------