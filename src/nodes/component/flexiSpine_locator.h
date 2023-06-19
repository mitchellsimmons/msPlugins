#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

#include <maya/MAnimControl.h>
#include <maya/MAnimCurveChange.h>
#include <maya/MArgDataBase.h>
#include <maya/MArgList.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MDGContext.h>
#include <maya/MDGModifier.h>
#include <maya/MEvaluationNode.h>
#include <maya/MFnDependencyNode.h>
#include <maya/MFnAnimCurve.h>
#include <maya/MFnToolContext.h>
#include <maya/MGlobal.h>
#include <maya/MItSelectionList.h>
#include <maya/MMatrix.h>
#include <maya/MDagMessage.h>
#include <maya/MObject.h>
#include <maya/MPlug.h>
#include <maya/MPlugArray.h>
#include <maya/MPxCommand.h>
#include <maya/MPxLocatorNode.h>
#include <maya/MSelectionList.h>
#include <maya/MStatus.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>
#include <maya/MTime.h>
#include <maya/MTypeId.h>
#include <maya/MVector.h>
#include <maya/MViewport2Renderer.h>
#include <maya/MQuaternion.h>

#include <SeExpr2/Curve.h>
#include <SeExpr2/Vec.h>

#include "flexiHelpers.h"
#include "data/eulerArray_data.h"
#include "utils/math_utils.h"
#include "utils/matrix_utils.h"
#include "utils/node_utils.h"
#include "utils/quaternion_utils.h"
#include "utils/spline_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Node models a non-rational B-spline curve (all basis weights = 1)
// Specifically an order = 4 BSpline (ie. made up of piecewise polynomial segments of degree = 3)
// Order = 4 is the smallest order which provides C2 continuity (slope + curvature)
class FlexiSpine : public MPxLocatorNode, MRS::NodeHelper
{
public:
	// ------ Data ------
	// Class designed to encapsulate data for FlexiSpine objects
	// Data is updated whenever the node evaluates
	class FlexiSpine_Data
	{
	private:
		friend class FlexiSpine;
		FlexiSpine_Data();
		~FlexiSpine_Data();

	public:
		// constants
		static const unsigned int order;
		static const unsigned int degree;

		// counts
		unsigned int outputCount;
		unsigned int parameterCount;
		unsigned int sampleCount;
		unsigned int subdivisions;

		// states
		bool isClosed;
		bool isDiscardLastEnabled;
		bool isOrientEnabled;
		bool isNormalUpVectorOverrideEnabled;
		bool isCounterTwistUpVectorOverrideEnabled;
		bool isScaleAdjustmentEnabled;
		bool isTwistAdjustmentEnabled;
		bool isDrawRibbonEnabled;
		
		// values
		double parameterizationBlend;
		double counterTwistBlend;
		double counterTwist;
		double startTwist;
		double endTwist;
		double roll;

		// knots
		std::vector<double> knots;
		double lowerBoundKnot;
		double upperBoundKnot;

		// parameters
		std::vector<double> naturalParameters;
		std::vector<double> arcLengthParameters;
		std::vector<double> blendedParameters;
		unsigned int minParamIndex;

		// lengths (natural parameter increments)
		std::vector<double> lengths;

		// sample xforms
		MVector vPrincipalNormal;
		std::vector<MVector> points;
		std::vector<MVector> tangents;
		std::vector<MVector> binormals;
		std::vector<MVector> normals;
		std::vector<MQuaternion> rmfReflections;

		// input xforms
		MVector vNormalUp;
		MVector vCounterTwistUp;
		std::vector<MVector> controlPoints;

		// output xforms
		std::vector<MMatrix> frames;

		// adjustments
		std::vector<TwistAdjustment> twistAdjustments;
		std::vector<ScaleAdjustment> scaleAdjustments;
	};

public:
	FlexiSpine();
	~FlexiSpine() override;

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;
	static const MString kDrawClassification;

	// ------ MPxNode ------
	SchedulingType schedulingType() const override;
	void postConstructor() override;
	static MStatus initialize();
	MStatus setDependentsDirty(const MPlug& plug, MPlugArray& plugArray) override;
	MStatus preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode) override;
	MStatus compute(const MPlug& plug, MDataBlock& dataBlock) override;

	// ------ MPxLocatorNode ------
	bool isBounded() const;
	MBoundingBox boundingBox() const;

	// ------ Helpers ------
	void computeCurveData(MDataBlock& dataBlock);
	void computeScaleAdjustments(MDataBlock& dataBlock);
	void computeTwistAdjustments(MDataBlock& dataBlock);
	MStatus computeNormalStability(double& outStability);
	MStatus computeCounterTwistStability(double& outStability);
	MStatus computeCounterTwist(MTime startTime, MTime endTime, MTime timeStep, MObject& animCurveObj, MAnimCurveChange& animMod);
	MVector sampleCurve(double t) const;
	MVector sampleFirstDerivative(double t) const;
	static void instancingChangedCallback(MDagPath& child, MDagPath& parent, void* clientData);

	const MRS::BSpline& getCurve() const;
	const FlexiSpine_Data& getCurveData() const;
	MDataBlock getDataBlock();

	// ------ Attr ------
	// inputs
	static MObject controlPointsAttr;
	static MObject upVectorAttr;
	static MObject upVectorXAttr;
	static MObject upVectorYAttr;
	static MObject upVectorZAttr;
	static MObject computeOrientationAttr;
	static MObject closeCurveAttr;
	static MObject discardLastOutputAttr;
	static MObject rotateOrderAttr;
	static MObject outputCountAttr;
	static MObject subdivisionsAttr;
	static MObject parameterizationBlendAttr;
	static MObject normalUpVectorOverrideCompoundAttr;
	static MObject normalUpVectorOverrideStateAttr;
	static MObject normalUpVectorOverrideAttr;
	static MObject normalUpVectorOverrideXAttr;
	static MObject normalUpVectorOverrideYAttr;
	static MObject normalUpVectorOverrideZAttr;
	static MObject counterTwistUpVectorOverrideCompoundAttr;
	static MObject counterTwistUpVectorOverrideStateAttr;
	static MObject counterTwistUpVectorOverrideAttr;
	static MObject counterTwistUpVectorOverrideXAttr;
	static MObject counterTwistUpVectorOverrideYAttr;
	static MObject counterTwistUpVectorOverrideZAttr;
	static MObject counterTwistBlendAttr;
	static MObject counterTwistAttr;
	static MObject startTwistAttr;
	static MObject endTwistAttr;
	static MObject rollAttr;
	static MObject drawRibbonAttr;
	static MObject drawCurveAttr;
	static MObject drawNormalsAttr;
	static MObject drawHullAttr;
	static MObject drawSpaceTransformationAttr;
	static MObject customDrawSpaceTransformAttr;
	static MObject computeTwistAdjustmentsAttr;
	static MObject twistAdjustmentCompoundAttr;
	static MObject twistAdjustmentRampAttr;
	static MObject twistAdjustmentRampPositionAttr;
	static MObject twistAdjustmentRampValueAttr;
	static MObject twistAdjustmentRampInterpolationAttr;
	static MObject twistAdjustmentValueAttr;
	static MObject twistAdjustmentOffsetAttr;
	static MObject twistAdjustmentRepeatAttr;
	static MObject twistAdjustmentFalloffModeAttr;
	static MObject twistAdjustmentFalloffDistanceAttr;
	static MObject computeScaleAdjustmentsAttr;
	static MObject scaleAdjustmentCompoundAttr;
	static MObject scaleAdjustmentRampAttr;
	static MObject scaleAdjustmentRampPositionAttr;
	static MObject scaleAdjustmentRampValueAttr;
	static MObject scaleAdjustmentRampInterpolationAttr;
	static MObject scaleAdjustmentValueAttr;
	static MObject scaleAdjustmentValueXAttr;
	static MObject scaleAdjustmentValueYAttr;
	static MObject scaleAdjustmentValueZAttr;
	static MObject scaleAdjustmentOffsetAttr;
	static MObject scaleAdjustmentRepeatAttr;
	static MObject scaleAdjustmentFalloffModeAttr;
	static MObject scaleAdjustmentFalloffDistanceAttr;
	// outputs
	static MObject outputLocalFramesAttr;
	static MObject outputLocalPositionsAttr;
	static MObject outputLocalOrientationsAttr;
	static MObject outputLocalScalesAttr;
	static MObject outputWorldFramesAttr;
	static MObject outputWorldPositionsAttr;
	static MObject outputWorldOrientationsAttr;
	static MObject outputWorldScalesAttr;
	// state trackers
	static MObject evalSinceDirtyAttr;
	static MObject drawSinceEvalAttr;

private:
	// ------ Curve ------
	MRS::BSpline m_curve;
	FlexiSpine_Data m_data;

	// ------ Callbacks ------
	MCallbackId m_instanceAddedCallbackId;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a command that will be used to calculate the counter twist cache
class FlexiSpine_CounterTwistCommand : public MPxCommand
{
public:
	FlexiSpine_CounterTwistCommand();
	~FlexiSpine_CounterTwistCommand() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ Const ------
	static const char* kStartTimeFlag;
	static const char* kStartTimeFlagLong;
	static const char* kEndTimeFlag;
	static const char* kEndTimeFlagLong;
	static const char* kTimeStepFlag;
	static const char* kTimeStepFlagLong;

	// ------ MPxCommand ------
	bool isUndoable() const override;
	MStatus doIt(const MArgList& args) override;
	MStatus redoIt() override;
	MStatus undoIt() override;

private:
	// ------ Data ------
	MAnimCurveChange m_animChangeMod;
	MDGModifier m_animDGMod;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Class defines a query only command that will be used to calculate the up-vector/counter-twist stability at a given time
class FlexiSpine_StabilityCommand : public MPxCommand
{
public:
	FlexiSpine_StabilityCommand();
	~FlexiSpine_StabilityCommand() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ Const ------
	static const char* kTimeFlag;
	static const char* kTimeFlagLong;
	static const char* kNormalFlag;
	static const char* kNormalFlagLong;
	static const char* kCounterTwistFlag;
	static const char* kCounterTwistFlagLong;

	// ------ MPxCommand ------
	MStatus doIt(const MArgList& args) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------