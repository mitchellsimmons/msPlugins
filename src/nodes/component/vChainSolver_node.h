#pragma once

#include <unordered_map>

#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDGContext.h>
#include <maya/MEvaluationNode.h>
#include <maya/MMatrix.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPxNode.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/math_utils.h"
#include "utils/matrix_utils.h"
#include "utils/node_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------



// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class VChainSolver : public MPxNode, MRS::NodeHelper
{
public:
	// ------ Data ------
	// Class designed to encapsulate data for VChainSolver objects
	struct Triangle
	{
	private:
		friend class VChainSolver;
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

	VChainSolver();
	~VChainSolver();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	enum Axis : short
	{
		kPosX = 0,
		kPosY = 1,
		kPosZ = 2,
		kNegX = 3,
		kNegY = 4,
		kNegZ = 5,
	};

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
	// Basis
	static MObject parentInverseFrameAttr;
	static MObject rootPositionAttr;
	static MObject handlePositionAttr;
	static MObject upVectorPositionAttr;
	static MObject aimAxisAttr;
	static MObject upAxisAttr;
	// Solver
	static MObject length0Attr;
	static MObject length1Attr;
	static MObject directionAttr;
	static MObject solverAttr;
	static MObject maxCompressionRatioAttr;
	static MObject maxRigidExtensionRatioAttr;
	static MObject maxNonRigidExtensionRatioAttr;
	static MObject maxHeightReductionRatioAttr;
	static MObject heightReductionDecelerationAttr;
	static MObject hierarchicalOutputAttr;
	// Output
	static MObject outputFramesAttr;
	static MObject outputRootToEffectorOffsetAttr;

private:
	// ------ Dirty Tracker ------
	bool m_evalSinceDirty;

	// ------ Data ------
	Triangle m_solution;
	std::vector<MMatrix> m_outputFrames;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------