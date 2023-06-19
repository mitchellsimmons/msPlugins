#pragma once

#include <cmath>
#include <unordered_map>

#include <maya/MDGContext.h>
#include <maya/MEvaluationManager.h>
#include <maya/MMatrix.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPxTransform.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>
#include <maya/MTypes.h>
#include <maya/MVector.h>

#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

#if MAYA_API_VERSION >= 20180000
#define VALIDATE_AND_SET_VALUE_PROTOYPE validateAndSetValue(const MPlug& plug, const MDataHandle& handle)
#define VALIDATE_AND_SET_VALUE_PARAMETERS plug, handle
#else
#define VALIDATE_AND_SET_VALUE_PROTOYPE validateAndSetValue(const MPlug& plug, const MDataHandle& handle, const MDGContext& context)
#define VALIDATE_AND_SET_VALUE_PARAMETERS plug, handle, context
#endif

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class AimMatrix : public MPxTransformationMatrix
{
public:
	AimMatrix();
	~AimMatrix();

	// ------ Const ------
	enum Axis : short
	{
		kPosX = 0,
		kPosY = 1,
		kPosZ = 2,
		kNegX = 3,
		kNegY = 4,
		kNegZ = 5,
	};

	const static MTypeId kTypeId;

	// ------ MPxTransformationMatrix ------
	MMatrix asMatrix() const override;
	MMatrix asMatrix(double percent) const override;

	// ------ Data ------
	MMatrix parentInverseFrame;
	MVector rootPosition;
	MVector aimPosition;
	MVector upPosition;
	Axis aimAxis;
	Axis upAxis;

private:
	// ------ Helpers ------
	MMatrix matrixFromInternals() const;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class AimTransform : public MPxTransform, MRS::NodeHelper
{
public:
	AimTransform();
	#if MAYA_API_VERSION < 20190000
	AimTransform(MPxTransformationMatrix* p_mtx);
	#endif
	~AimTransform();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxTransform ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MPxTransformationMatrix* createTransformationMatrix() override;
	MStatus VALIDATE_AND_SET_VALUE_PROTOYPE;
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

	// ------ Attr ------
	static MObject parentInverseFrameAttr;
	static MObject rootPositionAttr;
	static MObject aimPositionAttr;
	static MObject upPositionAttr;
	static MObject aimAxisAttr;
	static MObject upAxisAttr;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------