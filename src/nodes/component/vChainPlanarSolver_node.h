#pragma once

#include <unordered_map>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDGContext.h>
#include <maya/MEvaluationNode.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/math_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class VChainPlanarSolver : public MPxNode, MRS::NodeHelper
{
public:
	// ------ Data ------
	// Class designed to encapsulate data for VChainPlanarSolver objects
	struct Triangle
	{
	private:
		friend class VChainPlanarSolver;
		Triangle();
		~Triangle();

	public:
		double a;
		double b;
		double c;

		double A;
		double B;
		double C;
	};

	VChainPlanarSolver();
	~VChainPlanarSolver();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	enum Direction : short
	{
		kBackward = -1,
		kForward = 1,
	};

	enum Solver : short
	{
		kRigidSolver = 0,
		kLengthDampeningSolver = 1,
		kHeightLockExtensionSolver = 2,
		kHeightReductionExtensionSolver = 3,
	};

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	static MStatus initialize();
	MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray) override;
	MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;
	MStatus compute(const MPlug &plug, MDataBlock &dataBlock) override;

	// ------ Helpers ------
	void computeData(MDataBlock& dataBlock);
	void solveAngles(Triangle& triangle);

	// ------ Attr ------
	static MObject length0Attr;
	static MObject length1Attr;
	static MObject startToHandleOffsetAttr;
	static MObject directionAttr;
	static MObject solverAttr;
	static MObject maxCompressionRatioAttr;
	static MObject maxRigidExtensionRatioAttr;
	static MObject maxNonRigidExtensionRatioAttr;
	static MObject maxHeightReductionRatioAttr;
	static MObject heightReductionDecelerationAttr;
	static MObject outputAngle0Attr;
	static MObject outputAngle1Attr;
	static MObject outputAngle2Attr;
	static MObject outputLength0Attr;
	static MObject outputLength1Attr;
	static MObject outputStartToEffectorOffsetAttr;

private:
	// ------ Dirty Tracker ------
	bool m_evalSinceDirty;

	// ------ Data ------
	Triangle m_solution;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------