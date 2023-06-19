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

#include "data/quaternionArray_data.h"
#include "utils/node_utils.h"
#include "utils/quaternion_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class WeightedAverageQuaternion : public MPxNode, MRS::NodeHelper
{
public:
	WeightedAverageQuaternion();
	~WeightedAverageQuaternion();

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
	static MObject outputAttr;
	static MObject outputXAttr;
	static MObject outputYAttr;
	static MObject outputZAttr;
	static MObject outputWAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------