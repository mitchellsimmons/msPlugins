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

#include "utils/matrix_utils.h"
#include "utils/node_utils.h"
#include "utils/quaternion_utils.h"
#include "utils/vector_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class AverageMatrix : public MPxNode, MRS::NodeHelper
{
public:
	AverageMatrix();
	~AverageMatrix();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject inputAttr;
	static MObject outputAttr;

private:
	// ------ Data ------
	std::vector<MVector> m_translation;
	std::vector<MQuaternion> m_rotation;
	std::vector<MVector> m_scale;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------