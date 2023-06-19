#pragma once

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class GeometricSequenceDoubleArray : public MPxNode, MRS::NodeHelper
{
public:
	GeometricSequenceDoubleArray();
	~GeometricSequenceDoubleArray();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Attr ------
	static MObject firstTermAttr;
	static MObject commonRatioAttr;
	static MObject sizeAttr;
	static MObject outputSequenceAttr;

private:
	// ------ Data ------
	std::vector<double> m_outputSequence;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------