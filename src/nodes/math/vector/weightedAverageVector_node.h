#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>

#include "utils/node_utils.h"
#include "utils/vector_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class WeightedAverageVector : public MPxNode, MRS::NodeHelper
{
public:
	WeightedAverageVector();
	~WeightedAverageVector();

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
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------