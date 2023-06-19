#include "flexiChainDouble_locator.h"

/*	Description
	-----------
	This node approximates and draws two continuous cubic trigonemtric Bezier curves, each with two shaping parameters
	The normal vector is stabalised by calculating a rotational minimising frame at each sample point

	Considerations
	--------------
	MPxLocatorNode was chosen as the base over MPxSurfaceShape as we have no need for user assigned shaders (ie. I want full control over what is drawn)

	For drawing, MPxSubSceneOverride was chosen over other scene overrides for multiple reasons
	- Testing shows performance improvements of over 25% compared to MPxGeometryOverride
	- MPxGeometryOverride seems more suited for nodes which derive from MPxSurfaceShape (ie. when we want user assigned shaders)
	- MPxDrawOverride is well suited for simple UI drawables but for drawing render items its interface is probably too low level
	- MPxSubSceneOverride provides a very simple interface and does not place restrictions on when data can be accessed from Maya
	Adding even basic UI drawables tanks performance by up to 70% when compared to using MRenderItems

	There are two sets of output attributes used by this node
	Originally I considered having two input attributes which would specify the input and output spaces of transform data
	However there needs to be a seperation of concerns as we don't want local outputs to be affected by world space transformations
	- ie. We need to use setWorldSpace and setArray on world space outputs but not on local outputs

	The node needs to be able to draw whenever an input attribute becomes dirty
	However, output attributes will only evaluate when a pull request is made from downstream, not when an input causes dirty propagation
	The following methods provide safe access points to retrieve data from the DG during VP2 drawing:
	- MPxDrawOverride::prepareForDraw(), MPxGeometryOverride::updateDG()
	- MPxSubSceneOverride has no restrictions on accessing the DG
	We could access the relevant output attribute from one of these methods to ensure compute is called
	However, we should consider the implication to performance as compute will now be called for draw and graph eval

	Optimations
	-----------
	Instead, we can track when relevant inputs become dirty and only evaluate the curve when required
	The following methods allow us to track when attributes become dirty or require evaluation for DG and EM respectively
	- MPxNode::setDependendentsDirty(), MPxNode::preEvaluation()
	We can selectively evaluate the curve from either compute or from the scene overrider based upon the dirty state at the time of evaluation
	- If compute evaluates the curve, then draw needs to know an update has occurred since the last dirty

	If we are using MPxSubSceneOverride, we also want to track if a draw cycle has completed since the last evaluation
	- MPxSubSceneOverride draw cycles are triggered much more frequently (ie. viewport interaction is a trigger) compared to MPxGeometryOverride
	In this case, the dirty tracker only tells draw that fresh data is needed, it does not provide enough information about what triggered the call
	- A dirty input attribute may have caused setDependentsDirty to trigger a draw, with dirty state = true
	- Compute will trigger a draw, with dirty state = false
	- Viewport interaction will trigger a draw, with dirty state = false
	We do not want to update the buffers when viewport interaction causes a draw cycle but the dirty tracker alone does not tell us what triggered the update

	Computation of world space outputs has been optimised so that evaluation of the curve only occurs when the trigger was a dirty input attribute
	If a world space transformation triggered the call, we only need to recompute the world space transformation on the existing local data

	Known Issues
	------------
	When a hidden transform makes an indirect connection to this node (somewhere downstream), its shape will not inherit any of its transformations once unhidden
	- This only occurs in viewport 2.0 whilst EM is in use
	- Transform data is evaluated and propagated without error (possibly indicating a rendering issue)
	- It appears the draw cycles of connected nodes are not being triggered under these circumstances
	- Toggling the visibility of this node resolves the issue for all connected transforms that were hidden at the time of connection (including file open)

	Attributes
	----------
	controlPoint0, controlPoint1, controlPoint2, controlPoint3 - vector
		Specifies the four local space positions which determine the hull of the T-Bezier and help define its shape

	upVector - vector
		A local space vector which is used as the default up-vector in the following calculations:
		- Determining the principal normal of the curve
		- Determining the target vector for the counter-twist operation
		Only used when the "orientEnabled" attribute is true

	computeOrientation - bool
		Specifies whether to compute orientation data for the output attributes (local/world output frames and output transforms)

	outputCount - int [2, 500]
		Specifies how many outputs to produce

	subdivisions - int [0, 100]
		Specifies how many extra samples will be calculated between each pair of output points
		Acts as a precision multiplier for the following calculations:
		- Arc-length parameterization
		- Rotation minimization

	parameterization - enum
		Specifies how to parameterize the curve

		Natural				(0)		The curve will be parameterized using uniform time
		Arc Length			(1)		The curve will be parameterized in terms of the total arc length using uniform distance
		Split Arc Length	(2)		The curve will be split into three segments at the two joints
									Each segment will be parameterized in terms of the segment length using uniform distance

	rotateOrder - enum
		Specifies the composition order of rotations used to produce each of the euler values output by the orientation attributes

	jointOffset0, jointOffset1 - double
		Specifies a multiplier which is used to offset each of the virtual points away from the physical point
		- The offset direction is defined by the joint angle, such that rotating one edge half way towards the other defines the direction
		- The offset distance is proportional to both the given value and the joint angle
		- An angle of pi will act as a zero multiplier to the offset, whilst an angle of zero will produce a maximum offset

	jointRadius0, jointRadius1 - distance
		Specifies the distance of each virtual point away from the physical point
		Used to affect the curvature of the joint, whereby larger values result in a greater curvature
		- The offset direction is perpendicular to the jointOffset direction

	jointVolume0, jointVolume1 -double [-1.0, 1.0]
		Specifies the influence of the control polygon on the curve
		- A value of 0.0 represents that of the standard form cubic Bezier
		- A minimum of -1.0 decreases the influence of the control polygon on the curve
		- A maximum of 1.0 increases the influence of the control polygon on the curve

	normalUpVectorOverrideDirection - vector
		Specify an up-vector which can be used to temporarily override the default up-vector used in the principal normal calculation
		Designed to be used as a manual override when the tangent at the start of the curve becomes close to parallel with the default up-vector

	normalUpVectorOverrideState - bool
		Specify whether the up-vector override is enabled for the principal normal calculation

	counterTwistUpVectorOverrideDirection - vector
		Specify an up-vector which can be used to temporarily override the default up-vector used in the counter-twist calculation
		Designed to be used as a manual override when the tangent at the end of the curve becomes close to parallel with the default up-vector

	counterTwistUpVectorOverrideState - bool
		Specify whether the up-vector override is enabled for the counter-twist operation

	counterTwistBlend - double [0.0, 1.0]
		Specifies whether any twist deviation should be back-propagated down the curve in order to stabalize the end frame
		Only used when the "orientEnabled" attribute is true

	startTwist - angle
		Applies an additional twist to the start of the curve which is interpolated down its length via a falloff
		Only used when the "orientEnabled" attribute is true

	endTwist - angle
		Applies an additional twist to the end of the curve which is interpolated down its length via a falloff
		Only used when the "orientEnabled" attribute is true

	roll - angle
		Applies an additional constant twist to each output

	drawRibbon - bool
		Specifies whether to draw a ribbon, providing a visual representation of the output position and orientation data

	drawCurve - bool
		Specifies whether to draw a curve, providing a visual representation of the output position data, curve precision and curve parameterization

	drawNormals - bool
		Specifies whether to draw the curve normals for each output position

	drawHull - bool
		Specifies whether to draw the convex hull of the curve

	drawSpaceTransformation - enum
		Specifies the transformation used to draw each render item
		
		Local	(0)		Drawing will occur using the local data
		World	(1)		Drawing will occur relative to the world matrix of the locator node
		Custom	(2)		Drawing will occur relative to the transformation matrix provided by the customDrawSpaceTransform attribute

	customDrawSpaceTransform - matrix
		Specifies a transformation which will be applied to each render item if the "drawSpaceTransformation" attribute is set to "custom"
		Because outputs are in local space, this can be useful if you want drawing to occur relative to some transform

	computeTwistAdjustments, computeScaleAdjustments - bool
		Specifies whether to compute twist/scale adjustment data for the output attributes (local/world output frames and output transforms)

	twistAdjustmentRamp, scaleAdjustmentRamp - ramp
		Specify the falloff of a given twist/scale adjustment
		The falloff curve is mapped to points along the curve based on the attributes described below

	twistAdjustmentValue, scaleAdjustmentValue - double
		Specifies the maximum value of the falloff ramp, whereby a ramp value of 1.0 maps directly to the value given

	twistAdjustmentOffset, scaleAdjustmentOffset - double
		Specifies an offset in terms of the current parameterization which is used to map the central position of the falloff ramp to a position along the curve
		The central position of the falloff ramp is determined by the falloff mode described below

	twistAdjustmentFalloffMode, scaleAdjustmentFalloffMode - enum
		Specifies the falloff behaviour of the ramp, specifically where the central point of the ramp exists and the direction of the falloff from this point
		
		Forward		(0)		The ramp position 0.0 will map directly to the curve position defined by the offset
							A mapping is established, such that as the position along the ramp increments, the curve's parameter increments in the same direction
		Backward	(1)		The ramp position 0.0 will map directly to the curve position defined by the offset
							A mapping is established, such that as the position along the ramp increments, the curve's parameter increments in the opposite direction
		Mirror		(2)		The ramp position 0.0 will map directly to the curve position defined by the offset
							A mapping is established, such that as the position along the ramp increments, the curve's parameter increments in both directions
		Center		(3)		The ramp position 0.5 will map directly to the curve position defined by the offset
							A mapping is established, such that as the position along the ramp increments, the curve's parameter increments in the same direction

	twistAdjustmentFalloffDistance, scaleAdjustmentFalloffDistance - double
		Defines the distance in terms of a parameterization of arc-length to which the falloff ramp is mapped to positions along the curve

	twistAdjustmentRepeat, scaleAdjustmentRepeat - bool
		Specifies whether to repeat the adjustment ramp so that it covers the curve's entire domain

	outputLocalFrames - matrixArray
		Array of local space matrices sampled along the curve

	outputLocalPositions - vectorArray
		Array of local space positions sampled along the curve

	outputLocalOrientations - eulerArray
		Array of local space orientations sampled along the curve

	outputLocalScales - vectorArray
		Array of local space scales sampled along the curve

	outputWorldFrames - matrixArray[]
		Array of world space matrices sampled along the curve
		This node does not support instancing, therefore only the first element plug will allow a connection

	outputWorldPositions - vectorArray[]
		Array of world space positions sampled along the curve
		This node does not support instancing, therefore only the first element plug will allow a connection

	outputWorldOrientations - eulerArray[]
		Array of world space orientations sampled along the curve
		This node does not support instancing, therefore only the first element plug will allow a connection

	outputWorldScales - vectorArray[]
		Array of world space scales sampled along the curve
		This node does not support instancing, therefore only the first element plug will allow a connection
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FlexiChainDouble::FlexiChainDouble() :
	m_instanceAddedCallbackId{ 0 }
{}

FlexiChainDouble::~FlexiChainDouble()
{
	if (m_instanceAddedCallbackId != 0)
	{
		MMessage::removeCallback(m_instanceAddedCallbackId);
		m_instanceAddedCallbackId = 0;
	}
}

FlexiChainDouble::FlexiChainDouble_Data::FlexiChainDouble_Data() :
	outputCount{ 0 },
	parameterCount{ 0 },
	sampleCount{ 0 },
	subdivisions{ 0 },
	isOrientEnabled{ true },
	isNormalUpVectorOverrideEnabled{ false },
	isCounterTwistUpVectorOverrideEnabled{ false },
	isScaleAdjustmentEnabled{ false },
	isTwistAdjustmentEnabled{ false },
	isDrawRibbonEnabled{ true },
	parameterization{ 1 },
	counterTwistBlend{ 0.0 },
	counterTwist{ 0.0 },
	startTwist{ 0.0 },
	endTwist{ 0.0 },
	roll{ 0.0 },
	jointVolume0{ 0.0 },
	jointVolume1{ 0.0 },
	controlPoints0{ 4 },
	controlPoints1{ 4 },
	currentParameters{ nullptr }
{}

FlexiChainDouble::FlexiChainDouble_Data::~FlexiChainDouble_Data() {}

// ------ Constants ------
const unsigned int FlexiChainDouble::FlexiChainDouble_Data::order = 3;
const unsigned int FlexiChainDouble::FlexiChainDouble_Data::degree = 3;
const double FlexiChainDouble::FlexiChainDouble_Data::parameterRange = 2.0;

// ------ Attr ------
MObject FlexiChainDouble::controlPoint0Attr;
MObject FlexiChainDouble::controlPoint0XAttr;
MObject FlexiChainDouble::controlPoint0YAttr;
MObject FlexiChainDouble::controlPoint0ZAttr;
MObject FlexiChainDouble::controlPoint1Attr;
MObject FlexiChainDouble::controlPoint1XAttr;
MObject FlexiChainDouble::controlPoint1YAttr;
MObject FlexiChainDouble::controlPoint1ZAttr;
MObject FlexiChainDouble::controlPoint2Attr;
MObject FlexiChainDouble::controlPoint2XAttr;
MObject FlexiChainDouble::controlPoint2YAttr;
MObject FlexiChainDouble::controlPoint2ZAttr;
MObject FlexiChainDouble::controlPoint3Attr;
MObject FlexiChainDouble::controlPoint3XAttr;
MObject FlexiChainDouble::controlPoint3YAttr;
MObject FlexiChainDouble::controlPoint3ZAttr;
MObject FlexiChainDouble::jointOffset0Attr;
MObject FlexiChainDouble::jointRadius0Attr;
MObject FlexiChainDouble::jointVolume0Attr;
MObject FlexiChainDouble::jointOffset1Attr;
MObject FlexiChainDouble::jointRadius1Attr;
MObject FlexiChainDouble::jointVolume1Attr;
MObject FlexiChainDouble::upVectorAttr;
MObject FlexiChainDouble::upVectorXAttr;
MObject FlexiChainDouble::upVectorYAttr;
MObject FlexiChainDouble::upVectorZAttr;
MObject FlexiChainDouble::rotateOrderAttr;
MObject FlexiChainDouble::computeOrientationAttr;
MObject FlexiChainDouble::subdivisionsAttr;
MObject FlexiChainDouble::outputCountAttr;
MObject FlexiChainDouble::parameterizationAttr;
MObject FlexiChainDouble::normalUpVectorOverrideCompoundAttr;
MObject FlexiChainDouble::normalUpVectorOverrideStateAttr;
MObject FlexiChainDouble::normalUpVectorOverrideAttr;
MObject FlexiChainDouble::normalUpVectorOverrideXAttr;
MObject FlexiChainDouble::normalUpVectorOverrideYAttr;
MObject FlexiChainDouble::normalUpVectorOverrideZAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideCompoundAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideStateAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideXAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideYAttr;
MObject FlexiChainDouble::counterTwistUpVectorOverrideZAttr;
MObject FlexiChainDouble::counterTwistBlendAttr;
MObject FlexiChainDouble::counterTwistAttr;
MObject FlexiChainDouble::startTwistAttr;
MObject FlexiChainDouble::endTwistAttr;
MObject FlexiChainDouble::rollAttr;
MObject FlexiChainDouble::drawRibbonAttr;
MObject FlexiChainDouble::drawCurveAttr;
MObject FlexiChainDouble::drawNormalsAttr;
MObject FlexiChainDouble::drawHullAttr;
MObject FlexiChainDouble::drawSpaceTransformationAttr;
MObject FlexiChainDouble::customDrawSpaceTransformAttr;
MObject FlexiChainDouble::computeTwistAdjustmentsAttr;
MObject FlexiChainDouble::twistAdjustmentCompoundAttr;
MObject FlexiChainDouble::twistAdjustmentRampAttr;
MObject FlexiChainDouble::twistAdjustmentRampPositionAttr;
MObject FlexiChainDouble::twistAdjustmentRampValueAttr;
MObject FlexiChainDouble::twistAdjustmentRampInterpolationAttr;
MObject FlexiChainDouble::twistAdjustmentValueAttr;
MObject FlexiChainDouble::twistAdjustmentOffsetAttr;
MObject FlexiChainDouble::twistAdjustmentFalloffModeAttr;
MObject FlexiChainDouble::twistAdjustmentFalloffDistanceAttr;
MObject FlexiChainDouble::twistAdjustmentRepeatAttr;
MObject FlexiChainDouble::computeScaleAdjustmentsAttr;
MObject FlexiChainDouble::scaleAdjustmentCompoundAttr;
MObject FlexiChainDouble::scaleAdjustmentRampAttr;
MObject FlexiChainDouble::scaleAdjustmentRampPositionAttr;
MObject FlexiChainDouble::scaleAdjustmentRampValueAttr;
MObject FlexiChainDouble::scaleAdjustmentRampInterpolationAttr;
MObject FlexiChainDouble::scaleAdjustmentValueAttr;
MObject FlexiChainDouble::scaleAdjustmentValueXAttr;
MObject FlexiChainDouble::scaleAdjustmentValueYAttr;
MObject FlexiChainDouble::scaleAdjustmentValueZAttr;
MObject FlexiChainDouble::scaleAdjustmentOffsetAttr;
MObject FlexiChainDouble::scaleAdjustmentFalloffModeAttr;
MObject FlexiChainDouble::scaleAdjustmentFalloffDistanceAttr;
MObject FlexiChainDouble::scaleAdjustmentRepeatAttr;
MObject FlexiChainDouble::evalSinceDirtyAttr;
MObject FlexiChainDouble::drawSinceEvalAttr;
MObject FlexiChainDouble::outputLocalFramesAttr;
MObject FlexiChainDouble::outputLocalPositionsAttr;
MObject FlexiChainDouble::outputLocalOrientationsAttr;
MObject FlexiChainDouble::outputLocalScalesAttr;
MObject FlexiChainDouble::outputWorldFramesAttr;
MObject FlexiChainDouble::outputWorldPositionsAttr;
MObject FlexiChainDouble::outputWorldOrientationsAttr;
MObject FlexiChainDouble::outputWorldScalesAttr;

// ------ MPxNode ------
MPxNode::SchedulingType FlexiChainDouble::schedulingType() const
{
	return SchedulingType::kParallel;
}

void FlexiChainDouble::postConstructor()
{
	// Keeping this node simple, instancing is not supported
	MDagPath path;
	MDagPath::getAPathTo(thisMObject(), path);
	m_instanceAddedCallbackId = MDagMessage::addInstanceAddedDagPathCallback(path, &instancingChangedCallback);

	// Retrieve the child ramp attributes
	MFnCompoundAttribute fnCompoundTwistRamp{ twistAdjustmentRampAttr };
	for (unsigned int i = 0; i < fnCompoundTwistRamp.numChildren(); ++i)
	{
		MObject childAttr = fnCompoundTwistRamp.child(i);
		MFnAttribute fnAttrChild{ childAttr };
		if (fnAttrChild.name() == "twistAdjustmentRamp_Position")
			twistAdjustmentRampPositionAttr = childAttr;
		else if (fnAttrChild.name() == "twistAdjustmentRamp_FloatValue")
			twistAdjustmentRampValueAttr = childAttr;
		else if (fnAttrChild.name() == "twistAdjustmentRamp_Interp")
			twistAdjustmentRampInterpolationAttr = childAttr;
	}

	MFnCompoundAttribute fnCompoundScaleRamp{ scaleAdjustmentRampAttr };
	for (unsigned int i = 0; i < fnCompoundScaleRamp.numChildren(); ++i)
	{
		MObject childAttr = fnCompoundScaleRamp.child(i);
		MFnAttribute fnAttrChild{ childAttr };
		if (fnAttrChild.name() == "scaleAdjustmentRamp_Position")
			scaleAdjustmentRampPositionAttr = childAttr;
		else if (fnAttrChild.name() == "scaleAdjustmentRamp_FloatValue")
			scaleAdjustmentRampValueAttr = childAttr;
		else if (fnAttrChild.name() == "scaleAdjustmentRamp_Interp")
			scaleAdjustmentRampInterpolationAttr = childAttr;
	}
}

MStatus FlexiChainDouble::initialize()
{
	MVector controlPoint;
	MVector upVector{ 0,1,0 };
	MAngle twist;
	MMatrix drawTransform;
	std::unordered_map<const char*, short> parameterization{ {"Natural", 0}, {"Arc Length", 1}, {"Split Arc Length", 2} };
	std::unordered_map<const char*, short> rotateOrderFields{ {"xyz", 0}, {"yzx", 1}, {"zxy", 2}, {"xzy", 3}, {"yxz", 4}, {"zyx", 5} };
	std::unordered_map<const char*, short> drawSpaceFields{ {"Local", 0}, {"World", 1}, {"Custom", 2} };
	std::unordered_map<const char*, short> falloffModeFields{ {"Forward", 0}, {"Backward", 1}, {"Mirror", 2}, {"Center", 3} };
	MVector scaleVector{ 1,1,1 };
	std::vector<MMatrix> outputFrames;
	std::vector<MVector> outputPositions;
	std::vector<MEulerRotation> outputOrientations;
	std::vector<MVector> outputScales;

	// Input transforms
	createVectorAttribute(controlPoint0Attr, controlPoint0XAttr, controlPoint0YAttr, controlPoint0ZAttr, "controlPoint0", "controlPoint0",
		controlPoint, kDefaultPreset | kKeyable);
	createVectorAttribute(controlPoint1Attr, controlPoint1XAttr, controlPoint1YAttr, controlPoint1ZAttr, "controlPoint1", "controlPoint1",
		controlPoint, kDefaultPreset | kKeyable);
	createVectorAttribute(controlPoint2Attr, controlPoint2XAttr, controlPoint2YAttr, controlPoint2ZAttr, "controlPoint2", "controlPoint2",
		controlPoint, kDefaultPreset | kKeyable);
	createVectorAttribute(controlPoint3Attr, controlPoint3XAttr, controlPoint3YAttr, controlPoint3ZAttr, "controlPoint3", "controlPoint3",
		controlPoint, kDefaultPreset | kKeyable);
	createVectorAttribute(upVectorAttr, upVectorXAttr, upVectorYAttr, upVectorZAttr, "upVector", "upVector",
		upVector, kDefaultPreset | kKeyable);

	// The counter-twist calculation requires that orient computation persists over the entirety of an animation (ie. it cannot be keyable)
	createBoolAttribute(computeOrientationAttr, "computeOrientation", "computeOrientation", true, kDefaultPreset);
	// Rotate order does not need to trigger a draw or reevaluation of the curve data (ie. it does not affect the dirty tracker)
	createEnumAttribute(rotateOrderAttr, "rotateOrder", "rotateOrder", rotateOrderFields, 0, kDefaultPreset | kKeyable);
	createIntAttribute(outputCountAttr, "outputCount", "outputCount", 10, kDefaultPreset | kKeyable);
	setMinMax<int>(outputCountAttr, 2, 500);
	createIntAttribute(subdivisionsAttr, "subdivisions", "subdivisions", 20, kDefaultPreset | kKeyable);
	setMinMax<int>(subdivisionsAttr, 0, 100);
	createEnumAttribute(parameterizationAttr, "parameterization", "parameterization", parameterization, 2, kDefaultPreset | kKeyable);
	createDoubleAttribute(jointOffset0Attr, "jointOffset0", "jointOffset0", 0.0, kDefaultPreset | kKeyable);
	createDistanceAttribute(jointRadius0Attr, "jointRadius0", "jointRadius0", 10.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(jointVolume0Attr, "jointVolume0", "jointVolume0", 0.0, kDefaultPreset | kKeyable);
	setMinMax<double>(jointVolume0Attr, -1.0, 1.0);
	createDoubleAttribute(jointOffset1Attr, "jointOffset1", "jointOffset1", 0.0, kDefaultPreset | kKeyable);
	createDistanceAttribute(jointRadius1Attr, "jointRadius1", "jointRadius1", 10.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(jointVolume1Attr, "jointVolume1", "jointVolume1", 0.0, kDefaultPreset | kKeyable);
	setMinMax<double>(jointVolume1Attr, -1.0, 1.0);

	// Up-vector overrides
	createBoolAttribute(normalUpVectorOverrideStateAttr, "normalUpVectorOverrideState", "normalUpVectorOverrideState", false, kDefaultPreset | kKeyable);
	createVectorAttribute(normalUpVectorOverrideAttr, normalUpVectorOverrideXAttr, normalUpVectorOverrideYAttr, normalUpVectorOverrideZAttr,
		"normalUpVectorOverrideDirection", "normalUpVectorOverrideDirection", upVector, kDefaultPreset | kKeyable);
	std::vector<MObject> normalUpVectorOverrideChildren{ normalUpVectorOverrideStateAttr, normalUpVectorOverrideAttr };
	createCompoundAttribute(normalUpVectorOverrideCompoundAttr, normalUpVectorOverrideChildren, "normalUpVectorOverride", "normalUpVectorOverride", kDefaultPreset);
	createBoolAttribute(counterTwistUpVectorOverrideStateAttr,
		"counterTwistUpVectorOverrideState", "counterTwistUpVectorOverrideState", false, kDefaultPreset | kKeyable);
	createVectorAttribute(counterTwistUpVectorOverrideAttr, counterTwistUpVectorOverrideXAttr, counterTwistUpVectorOverrideYAttr, counterTwistUpVectorOverrideZAttr,
		"counterTwistUpVectorOverrideDirection", "counterTwistUpVectorOverrideDirection", upVector, kDefaultPreset | kKeyable);
	std::vector<MObject> counterTwistUpVectorOverrideChildren{ counterTwistUpVectorOverrideStateAttr, counterTwistUpVectorOverrideAttr };
	createCompoundAttribute(counterTwistUpVectorOverrideCompoundAttr, counterTwistUpVectorOverrideChildren,
		"counterTwistUpVectorOverride", "counterTwistUpVectorOverride", kDefaultPreset);

	// Twist
	createDoubleAttribute(counterTwistBlendAttr, "counterTwistBlend", "counterTwistBlend", 0.0, kDefaultPreset | kKeyable);
	setMinMax<double>(counterTwistBlendAttr, 0.0, 1.0);
	createAngleAttribute(counterTwistAttr, "counterTwist", "counterTwist", twist, kDefaultPreset | kKeyable);
	createAngleAttribute(startTwistAttr, "startTwist", "startTwist", twist, kDefaultPreset | kKeyable);
	createAngleAttribute(endTwistAttr, "endTwist", "endTwist", twist, kDefaultPreset | kKeyable);
	createAngleAttribute(rollAttr, "roll", "roll", twist, kDefaultPreset | kKeyable);

	// Draw attributes must not have any affects relation (ie. they do not trigger compute)
	createBoolAttribute(drawRibbonAttr, "drawRibbon", "drawRibbon", true, kDefaultPreset | kKeyable);
	createBoolAttribute(drawCurveAttr, "drawCurve", "drawCurve", true, kDefaultPreset | kKeyable);
	createBoolAttribute(drawNormalsAttr, "drawNormals", "drawNormals", true, kDefaultPreset | kKeyable);
	createBoolAttribute(drawHullAttr, "drawHull", "drawHull", true, kDefaultPreset | kKeyable);
	createEnumAttribute(drawSpaceTransformationAttr, "drawSpaceTransformation", "drawSpaceTransformation", drawSpaceFields, 1, kDefaultPreset | kKeyable);
	createMatrixAttribute(customDrawSpaceTransformAttr, "customDrawSpaceTransform", "customDrawSpaceTransform", drawTransform, kDefaultPreset);

	// Twist adjustments
	createBoolAttribute(computeTwistAdjustmentsAttr, "computeTwistAdjustments", "computeTwistAdjustments", false, kDefaultPreset | kKeyable);
	createCurveRampAttribute(twistAdjustmentRampAttr, "twistAdjustmentRamp", "twistAdjustmentRamp", kArrayPreset);
	createAngleAttribute(twistAdjustmentValueAttr, "twistAdjustmentValue", "twistAdjustmentValue", twist, kDefaultPreset | kKeyable);
	createDoubleAttribute(twistAdjustmentOffsetAttr, "twistAdjustmentOffset", "twistAdjustmentOffset", 0.0, kDefaultPreset | kKeyable);
	createEnumAttribute(twistAdjustmentFalloffModeAttr, "twistAdjustmentFalloffMode", "twistAdjustmentFalloffMode", falloffModeFields, 0, kDefaultPreset | kKeyable);
	createDoubleAttribute(twistAdjustmentFalloffDistanceAttr, "twistAdjustmentFalloffDistance", "twistAdjustmentFalloffDistance", 1.0, kDefaultPreset | kKeyable);
	setMin<double>(twistAdjustmentFalloffDistanceAttr, 0.0);
	createBoolAttribute(twistAdjustmentRepeatAttr, "twistAdjustmentRepeat", "twistAdjustmentRepeat", false, kDefaultPreset | kKeyable);
	std::vector<MObject> twistAdjustmentCompoundChildren{ twistAdjustmentRampAttr, twistAdjustmentValueAttr,
		twistAdjustmentOffsetAttr, twistAdjustmentFalloffModeAttr, twistAdjustmentFalloffDistanceAttr, twistAdjustmentRepeatAttr };
	createCompoundAttribute(twistAdjustmentCompoundAttr, twistAdjustmentCompoundChildren, "twistAdjustment", "twistAdjustment", kArrayPreset);

	// Scale adjustments
	createBoolAttribute(computeScaleAdjustmentsAttr, "computeScaleAdjustments", "computeScaleAdjustments", false, kDefaultPreset | kKeyable);
	createCurveRampAttribute(scaleAdjustmentRampAttr, "scaleAdjustmentRamp", "scaleAdjustmentRamp", kArrayPreset);
	createVectorAttribute(scaleAdjustmentValueAttr, scaleAdjustmentValueXAttr, scaleAdjustmentValueYAttr, scaleAdjustmentValueZAttr,
		"scaleAdjustmentValue", "scaleAdjustmentValue", scaleVector, kDefaultPreset | kKeyable);
	createDoubleAttribute(scaleAdjustmentOffsetAttr, "scaleAdjustmentOffset", "scaleAdjustmentOffset", 0.0, kDefaultPreset | kKeyable);
	createEnumAttribute(scaleAdjustmentFalloffModeAttr, "scaleAdjustmentFalloffMode", "scaleAdjustmentFalloffMode", falloffModeFields, 0, kDefaultPreset | kKeyable);
	createDoubleAttribute(scaleAdjustmentFalloffDistanceAttr, "scaleAdjustmentFalloffDistance", "scaleAdjustmentFalloffDistance", 1.0, kDefaultPreset | kKeyable);
	setMin<double>(scaleAdjustmentFalloffDistanceAttr, 0.0);
	createBoolAttribute(scaleAdjustmentRepeatAttr, "scaleAdjustmentRepeat", "scaleAdjustmentRepeat", false, kDefaultPreset | kKeyable);
	std::vector<MObject> scaleAdjustmentCompoundChildren{ scaleAdjustmentRampAttr, scaleAdjustmentValueAttr,
		scaleAdjustmentOffsetAttr, scaleAdjustmentFalloffModeAttr, scaleAdjustmentFalloffDistanceAttr, scaleAdjustmentRepeatAttr };
	createCompoundAttribute(scaleAdjustmentCompoundAttr, scaleAdjustmentCompoundChildren, "scaleAdjustment", "scaleAdjustment", kArrayPreset);

	// State trackers are also internal only attributes
	createBoolAttribute(evalSinceDirtyAttr, "evalSinceDirty", "evalSinceDirty", false, kReadable | kWritable | kCached | kHidden);
	createBoolAttribute(drawSinceEvalAttr, "drawSinceEval", "drawSinceEval", false, kReadable | kWritable | kCached | kHidden);

	// Output transforms
	createMatrixDataArrayAttribute(outputLocalFramesAttr, "outputLocalFrames", "outputLocalFrames", outputFrames, kReadOnlyPreset);
	createVectorDataArrayAttribute(outputLocalPositionsAttr, "outputLocalPositions", "outputLocalPositions", outputPositions, kReadOnlyPreset);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputLocalOrientationsAttr, "outputLocalOrientations", "outputLocalOrientations",
		outputOrientations, kReadOnlyPreset);
	createVectorDataArrayAttribute(outputLocalScalesAttr, "outputLocalScales", "outputLocalScales", outputScales, kReadOnlyPreset);

	// World attributes must set: kWorldSpace, kArray, kUsesArrayDataBuilder
	createMatrixDataArrayAttribute(outputWorldFramesAttr, "outputWorldFrames", "outputWorldFrames", outputFrames, kReadOnlyArrayPreset | kWorldSpace);
	createVectorDataArrayAttribute(outputWorldPositionsAttr, "outputWorldPositions", "outputWorldPositions", outputPositions, kReadOnlyArrayPreset | kWorldSpace);
	createPluginDataArrayAttribute<EulerArrayData, MEulerRotation>(outputWorldOrientationsAttr, "outputWorldOrientations", "outputWorldOrientations",
		outputOrientations, kReadOnlyArrayPreset | kWorldSpace);
	createVectorDataArrayAttribute(outputWorldScalesAttr, "outputWorldScales", "outputWorldScales", outputScales, kReadOnlyArrayPreset | kWorldSpace);

	addAttribute(controlPoint0Attr);
	addAttribute(controlPoint1Attr);
	addAttribute(controlPoint2Attr);
	addAttribute(controlPoint3Attr);
	addAttribute(upVectorAttr);
	addAttribute(computeOrientationAttr);
	addAttribute(rotateOrderAttr);
	addAttribute(outputCountAttr);
	addAttribute(subdivisionsAttr);
	addAttribute(parameterizationAttr);
	addAttribute(jointOffset0Attr);
	addAttribute(jointRadius0Attr);
	addAttribute(jointVolume0Attr);
	addAttribute(jointOffset1Attr);
	addAttribute(jointRadius1Attr);
	addAttribute(jointVolume1Attr);
	addAttribute(normalUpVectorOverrideCompoundAttr);
	addAttribute(counterTwistUpVectorOverrideCompoundAttr);
	addAttribute(counterTwistBlendAttr);
	addAttribute(counterTwistAttr);
	addAttribute(startTwistAttr);
	addAttribute(endTwistAttr);
	addAttribute(rollAttr);
	addAttribute(drawRibbonAttr);
	addAttribute(drawCurveAttr);
	addAttribute(drawNormalsAttr);
	addAttribute(drawHullAttr);
	addAttribute(drawSpaceTransformationAttr);
	addAttribute(customDrawSpaceTransformAttr);
	addAttribute(computeTwistAdjustmentsAttr);
	addAttribute(twistAdjustmentCompoundAttr);
	addAttribute(computeScaleAdjustmentsAttr);
	addAttribute(scaleAdjustmentCompoundAttr);
	addAttribute(evalSinceDirtyAttr);
	addAttribute(drawSinceEvalAttr);
	addAttribute(outputLocalFramesAttr);
	addAttribute(outputLocalPositionsAttr);
	addAttribute(outputLocalOrientationsAttr);
	addAttribute(outputLocalScalesAttr);
	addAttribute(outputWorldFramesAttr);
	addAttribute(outputWorldPositionsAttr);
	addAttribute(outputWorldOrientationsAttr);
	addAttribute(outputWorldScalesAttr);

	// Local Affects
	attributeAffects(controlPoint0Attr, outputLocalFramesAttr);
	attributeAffects(controlPoint1Attr, outputLocalFramesAttr);
	attributeAffects(controlPoint2Attr, outputLocalFramesAttr);
	attributeAffects(controlPoint3Attr, outputLocalFramesAttr);
	attributeAffects(upVectorAttr, outputLocalFramesAttr);
	attributeAffects(upVectorXAttr, outputLocalFramesAttr);
	attributeAffects(upVectorYAttr, outputLocalFramesAttr);
	attributeAffects(upVectorZAttr, outputLocalFramesAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputLocalFramesAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputLocalFramesAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputLocalFramesAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputLocalFramesAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputLocalFramesAttr);
	attributeAffects(computeOrientationAttr, outputLocalFramesAttr);
	attributeAffects(outputCountAttr, outputLocalFramesAttr);
	attributeAffects(subdivisionsAttr, outputLocalFramesAttr);
	attributeAffects(parameterizationAttr, outputLocalFramesAttr);
	attributeAffects(jointOffset0Attr, outputLocalFramesAttr);
	attributeAffects(jointRadius0Attr, outputLocalFramesAttr);
	attributeAffects(jointVolume0Attr, outputLocalFramesAttr);
	attributeAffects(jointOffset1Attr, outputLocalFramesAttr);
	attributeAffects(jointRadius1Attr, outputLocalFramesAttr);
	attributeAffects(jointVolume1Attr, outputLocalFramesAttr);
	attributeAffects(counterTwistBlendAttr, outputLocalFramesAttr);
	attributeAffects(counterTwistAttr, outputLocalFramesAttr);
	attributeAffects(startTwistAttr, outputLocalFramesAttr);
	attributeAffects(endTwistAttr, outputLocalFramesAttr);
	attributeAffects(rollAttr, outputLocalFramesAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputLocalFramesAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputLocalFramesAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputLocalFramesAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputLocalFramesAttr);

	attributeAffects(controlPoint0Attr, outputLocalPositionsAttr);
	attributeAffects(controlPoint1Attr, outputLocalPositionsAttr);
	attributeAffects(controlPoint2Attr, outputLocalPositionsAttr);
	attributeAffects(controlPoint3Attr, outputLocalPositionsAttr);
	attributeAffects(upVectorAttr, outputLocalPositionsAttr);
	attributeAffects(upVectorXAttr, outputLocalPositionsAttr);
	attributeAffects(upVectorYAttr, outputLocalPositionsAttr);
	attributeAffects(upVectorZAttr, outputLocalPositionsAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputLocalPositionsAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputLocalPositionsAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputLocalPositionsAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputLocalPositionsAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputLocalPositionsAttr);
	attributeAffects(computeOrientationAttr, outputLocalPositionsAttr);
	attributeAffects(outputCountAttr, outputLocalPositionsAttr);
	attributeAffects(subdivisionsAttr, outputLocalPositionsAttr);
	attributeAffects(parameterizationAttr, outputLocalPositionsAttr);
	attributeAffects(jointOffset0Attr, outputLocalPositionsAttr);
	attributeAffects(jointRadius0Attr, outputLocalPositionsAttr);
	attributeAffects(jointVolume0Attr, outputLocalPositionsAttr);
	attributeAffects(jointOffset1Attr, outputLocalPositionsAttr);
	attributeAffects(jointRadius1Attr, outputLocalPositionsAttr);
	attributeAffects(jointVolume1Attr, outputLocalPositionsAttr);
	attributeAffects(counterTwistBlendAttr, outputLocalPositionsAttr);
	attributeAffects(counterTwistAttr, outputLocalPositionsAttr);
	attributeAffects(startTwistAttr, outputLocalPositionsAttr);
	attributeAffects(endTwistAttr, outputLocalPositionsAttr);
	attributeAffects(rollAttr, outputLocalPositionsAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputLocalPositionsAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputLocalPositionsAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputLocalPositionsAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputLocalPositionsAttr);

	attributeAffects(controlPoint0Attr, outputLocalOrientationsAttr);
	attributeAffects(controlPoint1Attr, outputLocalOrientationsAttr);
	attributeAffects(controlPoint2Attr, outputLocalOrientationsAttr);
	attributeAffects(controlPoint3Attr, outputLocalOrientationsAttr);
	attributeAffects(upVectorAttr, outputLocalOrientationsAttr);
	attributeAffects(upVectorXAttr, outputLocalOrientationsAttr);
	attributeAffects(upVectorYAttr, outputLocalOrientationsAttr);
	attributeAffects(upVectorZAttr, outputLocalOrientationsAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputLocalOrientationsAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputLocalOrientationsAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputLocalOrientationsAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputLocalOrientationsAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputLocalOrientationsAttr);
	attributeAffects(computeOrientationAttr, outputLocalOrientationsAttr);
	attributeAffects(rotateOrderAttr, outputLocalOrientationsAttr);
	attributeAffects(outputCountAttr, outputLocalOrientationsAttr);
	attributeAffects(subdivisionsAttr, outputLocalOrientationsAttr);
	attributeAffects(parameterizationAttr, outputLocalOrientationsAttr);
	attributeAffects(jointOffset0Attr, outputLocalOrientationsAttr);
	attributeAffects(jointRadius0Attr, outputLocalOrientationsAttr);
	attributeAffects(jointVolume0Attr, outputLocalOrientationsAttr);
	attributeAffects(jointOffset1Attr, outputLocalOrientationsAttr);
	attributeAffects(jointRadius1Attr, outputLocalOrientationsAttr);
	attributeAffects(jointVolume1Attr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistBlendAttr, outputLocalOrientationsAttr);
	attributeAffects(counterTwistAttr, outputLocalOrientationsAttr);
	attributeAffects(startTwistAttr, outputLocalOrientationsAttr);
	attributeAffects(endTwistAttr, outputLocalOrientationsAttr);
	attributeAffects(rollAttr, outputLocalOrientationsAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputLocalOrientationsAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputLocalOrientationsAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputLocalOrientationsAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputLocalOrientationsAttr);

	attributeAffects(controlPoint0Attr, outputLocalScalesAttr);
	attributeAffects(controlPoint1Attr, outputLocalScalesAttr);
	attributeAffects(controlPoint2Attr, outputLocalScalesAttr);
	attributeAffects(controlPoint3Attr, outputLocalScalesAttr);
	attributeAffects(upVectorAttr, outputLocalScalesAttr);
	attributeAffects(upVectorXAttr, outputLocalScalesAttr);
	attributeAffects(upVectorYAttr, outputLocalScalesAttr);
	attributeAffects(upVectorZAttr, outputLocalScalesAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputLocalScalesAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputLocalScalesAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputLocalScalesAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputLocalScalesAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputLocalScalesAttr);
	attributeAffects(computeOrientationAttr, outputLocalScalesAttr);
	attributeAffects(rotateOrderAttr, outputLocalScalesAttr);
	attributeAffects(outputCountAttr, outputLocalScalesAttr);
	attributeAffects(subdivisionsAttr, outputLocalScalesAttr);
	attributeAffects(parameterizationAttr, outputLocalScalesAttr);
	attributeAffects(jointOffset0Attr, outputLocalScalesAttr);
	attributeAffects(jointRadius0Attr, outputLocalScalesAttr);
	attributeAffects(jointVolume0Attr, outputLocalScalesAttr);
	attributeAffects(jointOffset1Attr, outputLocalScalesAttr);
	attributeAffects(jointRadius1Attr, outputLocalScalesAttr);
	attributeAffects(jointVolume1Attr, outputLocalScalesAttr);
	attributeAffects(counterTwistBlendAttr, outputLocalScalesAttr);
	attributeAffects(counterTwistAttr, outputLocalScalesAttr);
	attributeAffects(startTwistAttr, outputLocalScalesAttr);
	attributeAffects(endTwistAttr, outputLocalScalesAttr);
	attributeAffects(rollAttr, outputLocalScalesAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputLocalScalesAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputLocalScalesAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputLocalScalesAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputLocalScalesAttr);

	// World Affects
	attributeAffects(controlPoint0Attr, outputWorldFramesAttr);
	attributeAffects(controlPoint1Attr, outputWorldFramesAttr);
	attributeAffects(controlPoint2Attr, outputWorldFramesAttr);
	attributeAffects(controlPoint3Attr, outputWorldFramesAttr);
	attributeAffects(upVectorAttr, outputWorldFramesAttr);
	attributeAffects(upVectorXAttr, outputWorldFramesAttr);
	attributeAffects(upVectorYAttr, outputWorldFramesAttr);
	attributeAffects(upVectorZAttr, outputWorldFramesAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputWorldFramesAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputWorldFramesAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputWorldFramesAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputWorldFramesAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputWorldFramesAttr);
	attributeAffects(computeOrientationAttr, outputWorldFramesAttr);
	attributeAffects(outputCountAttr, outputWorldFramesAttr);
	attributeAffects(subdivisionsAttr, outputWorldFramesAttr);
	attributeAffects(parameterizationAttr, outputWorldFramesAttr);
	attributeAffects(jointOffset0Attr, outputWorldFramesAttr);
	attributeAffects(jointRadius0Attr, outputWorldFramesAttr);
	attributeAffects(jointVolume0Attr, outputWorldFramesAttr);
	attributeAffects(jointOffset1Attr, outputWorldFramesAttr);
	attributeAffects(jointRadius1Attr, outputWorldFramesAttr);
	attributeAffects(jointVolume1Attr, outputWorldFramesAttr);
	attributeAffects(counterTwistBlendAttr, outputWorldFramesAttr);
	attributeAffects(counterTwistAttr, outputWorldFramesAttr);
	attributeAffects(startTwistAttr, outputWorldFramesAttr);
	attributeAffects(endTwistAttr, outputWorldFramesAttr);
	attributeAffects(rollAttr, outputWorldFramesAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputWorldFramesAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputWorldFramesAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputWorldFramesAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputWorldFramesAttr);

	attributeAffects(controlPoint0Attr, outputWorldPositionsAttr);
	attributeAffects(controlPoint1Attr, outputWorldPositionsAttr);
	attributeAffects(controlPoint2Attr, outputWorldPositionsAttr);
	attributeAffects(controlPoint3Attr, outputWorldPositionsAttr);
	attributeAffects(upVectorAttr, outputWorldPositionsAttr);
	attributeAffects(upVectorXAttr, outputWorldPositionsAttr);
	attributeAffects(upVectorYAttr, outputWorldPositionsAttr);
	attributeAffects(upVectorZAttr, outputWorldPositionsAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputWorldPositionsAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputWorldPositionsAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputWorldPositionsAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputWorldPositionsAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputWorldPositionsAttr);
	attributeAffects(computeOrientationAttr, outputWorldPositionsAttr);
	attributeAffects(outputCountAttr, outputWorldPositionsAttr);
	attributeAffects(subdivisionsAttr, outputWorldPositionsAttr);
	attributeAffects(parameterizationAttr, outputWorldPositionsAttr);
	attributeAffects(jointOffset0Attr, outputWorldPositionsAttr);
	attributeAffects(jointRadius0Attr, outputWorldPositionsAttr);
	attributeAffects(jointVolume0Attr, outputWorldPositionsAttr);
	attributeAffects(jointOffset1Attr, outputWorldPositionsAttr);
	attributeAffects(jointRadius1Attr, outputWorldPositionsAttr);
	attributeAffects(jointVolume1Attr, outputWorldPositionsAttr);
	attributeAffects(counterTwistBlendAttr, outputWorldPositionsAttr);
	attributeAffects(counterTwistAttr, outputWorldPositionsAttr);
	attributeAffects(startTwistAttr, outputWorldPositionsAttr);
	attributeAffects(endTwistAttr, outputWorldPositionsAttr);
	attributeAffects(rollAttr, outputWorldPositionsAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputWorldPositionsAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputWorldPositionsAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputWorldPositionsAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputWorldPositionsAttr);

	attributeAffects(controlPoint0Attr, outputWorldOrientationsAttr);
	attributeAffects(controlPoint1Attr, outputWorldOrientationsAttr);
	attributeAffects(controlPoint2Attr, outputWorldOrientationsAttr);
	attributeAffects(controlPoint3Attr, outputWorldOrientationsAttr);
	attributeAffects(upVectorAttr, outputWorldOrientationsAttr);
	attributeAffects(upVectorXAttr, outputWorldOrientationsAttr);
	attributeAffects(upVectorYAttr, outputWorldOrientationsAttr);
	attributeAffects(upVectorZAttr, outputWorldOrientationsAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputWorldOrientationsAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputWorldOrientationsAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputWorldOrientationsAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputWorldOrientationsAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputWorldOrientationsAttr);
	attributeAffects(computeOrientationAttr, outputWorldOrientationsAttr);
	attributeAffects(rotateOrderAttr, outputWorldOrientationsAttr);
	attributeAffects(outputCountAttr, outputWorldOrientationsAttr);
	attributeAffects(subdivisionsAttr, outputWorldOrientationsAttr);
	attributeAffects(parameterizationAttr, outputWorldOrientationsAttr);
	attributeAffects(jointOffset0Attr, outputWorldOrientationsAttr);
	attributeAffects(jointRadius0Attr, outputWorldOrientationsAttr);
	attributeAffects(jointVolume0Attr, outputWorldOrientationsAttr);
	attributeAffects(jointOffset1Attr, outputWorldOrientationsAttr);
	attributeAffects(jointRadius1Attr, outputWorldOrientationsAttr);
	attributeAffects(jointVolume1Attr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistBlendAttr, outputWorldOrientationsAttr);
	attributeAffects(counterTwistAttr, outputWorldOrientationsAttr);
	attributeAffects(startTwistAttr, outputWorldOrientationsAttr);
	attributeAffects(endTwistAttr, outputWorldOrientationsAttr);
	attributeAffects(rollAttr, outputWorldOrientationsAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputWorldOrientationsAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputWorldOrientationsAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputWorldOrientationsAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputWorldOrientationsAttr);

	attributeAffects(controlPoint0Attr, outputWorldScalesAttr);
	attributeAffects(controlPoint1Attr, outputWorldScalesAttr);
	attributeAffects(controlPoint2Attr, outputWorldScalesAttr);
	attributeAffects(controlPoint3Attr, outputWorldScalesAttr);
	attributeAffects(upVectorAttr, outputWorldScalesAttr);
	attributeAffects(upVectorXAttr, outputWorldScalesAttr);
	attributeAffects(upVectorYAttr, outputWorldScalesAttr);
	attributeAffects(upVectorZAttr, outputWorldScalesAttr);
	attributeAffects(normalUpVectorOverrideAttr, outputWorldScalesAttr);
	attributeAffects(normalUpVectorOverrideXAttr, outputWorldScalesAttr);
	attributeAffects(normalUpVectorOverrideYAttr, outputWorldScalesAttr);
	attributeAffects(normalUpVectorOverrideZAttr, outputWorldScalesAttr);
	attributeAffects(normalUpVectorOverrideStateAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideXAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideYAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideZAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistUpVectorOverrideStateAttr, outputWorldScalesAttr);
	attributeAffects(computeOrientationAttr, outputWorldScalesAttr);
	attributeAffects(rotateOrderAttr, outputWorldScalesAttr);
	attributeAffects(outputCountAttr, outputWorldScalesAttr);
	attributeAffects(subdivisionsAttr, outputWorldScalesAttr);
	attributeAffects(parameterizationAttr, outputWorldScalesAttr);
	attributeAffects(jointOffset0Attr, outputWorldScalesAttr);
	attributeAffects(jointRadius0Attr, outputWorldScalesAttr);
	attributeAffects(jointVolume0Attr, outputWorldScalesAttr);
	attributeAffects(jointOffset1Attr, outputWorldScalesAttr);
	attributeAffects(jointRadius1Attr, outputWorldScalesAttr);
	attributeAffects(jointVolume1Attr, outputWorldScalesAttr);
	attributeAffects(counterTwistBlendAttr, outputWorldScalesAttr);
	attributeAffects(counterTwistAttr, outputWorldScalesAttr);
	attributeAffects(startTwistAttr, outputWorldScalesAttr);
	attributeAffects(endTwistAttr, outputWorldScalesAttr);
	attributeAffects(rollAttr, outputWorldScalesAttr);
	attributeAffects(computeTwistAdjustmentsAttr, outputWorldScalesAttr);
	attributeAffects(twistAdjustmentCompoundAttr, outputWorldScalesAttr);
	attributeAffects(computeScaleAdjustmentsAttr, outputWorldScalesAttr);
	attributeAffects(scaleAdjustmentCompoundAttr, outputWorldScalesAttr);

	return MStatus::kSuccess;
}

/*	Description
	-----------
	Ensure the draw state of the node is dirtied when an input becomes dirty
	It is then up to the draw override to ensure data is pulled from the graph and is up to date
	ie. A dirty plug does not mean the node will automatically recompute as this only occurs when something downstream is pulling    */
MStatus FlexiChainDouble::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	// Highest priority attributes first
	if (plug == controlPoint0Attr || plug == controlPoint0XAttr || plug == controlPoint0YAttr || plug == controlPoint0ZAttr ||
		plug == controlPoint1Attr || plug == controlPoint1XAttr || plug == controlPoint1YAttr || plug == controlPoint1ZAttr ||
		plug == controlPoint2Attr || plug == controlPoint2XAttr || plug == controlPoint2YAttr || plug == controlPoint2ZAttr ||
		plug == controlPoint3Attr || plug == controlPoint3XAttr || plug == controlPoint3YAttr || plug == controlPoint3ZAttr ||
		plug == upVectorAttr || plug == upVectorXAttr || plug == upVectorYAttr || plug == upVectorZAttr ||
		plug == startTwistAttr ||
		plug == endTwistAttr ||
		plug == counterTwistAttr ||
		plug == rollAttr ||
		plug == jointOffset0Attr ||
		plug == jointRadius0Attr ||
		plug == jointVolume0Attr ||
		plug == jointOffset1Attr ||
		plug == jointRadius1Attr ||
		plug == jointVolume1Attr ||
		plug == normalUpVectorOverrideAttr || plug == normalUpVectorOverrideXAttr || 
		plug == normalUpVectorOverrideYAttr || plug == normalUpVectorOverrideZAttr ||
		plug == counterTwistUpVectorOverrideAttr || plug == counterTwistUpVectorOverrideXAttr ||
		plug == counterTwistUpVectorOverrideYAttr || plug == counterTwistUpVectorOverrideZAttr ||
		plug == normalUpVectorOverrideStateAttr ||
		plug == counterTwistUpVectorOverrideStateAttr ||
		plug == twistAdjustmentCompoundAttr ||
		plug == twistAdjustmentRampAttr ||
		plug == twistAdjustmentRampPositionAttr ||
		plug == twistAdjustmentRampValueAttr ||
		plug == twistAdjustmentRampInterpolationAttr ||
		plug == twistAdjustmentValueAttr ||
		plug == twistAdjustmentOffsetAttr ||
		plug == twistAdjustmentFalloffModeAttr ||
		plug == twistAdjustmentFalloffDistanceAttr ||
		plug == twistAdjustmentRepeatAttr ||
		plug == scaleAdjustmentCompoundAttr ||
		plug == scaleAdjustmentRampAttr ||
		plug == scaleAdjustmentRampPositionAttr ||
		plug == scaleAdjustmentRampValueAttr ||
		plug == scaleAdjustmentRampInterpolationAttr ||
		plug == scaleAdjustmentValueAttr || plug == scaleAdjustmentValueXAttr || 
		plug == scaleAdjustmentValueYAttr || plug == scaleAdjustmentValueZAttr ||
		plug == scaleAdjustmentOffsetAttr ||
		plug == scaleAdjustmentFalloffModeAttr ||
		plug == scaleAdjustmentFalloffDistanceAttr ||
		plug == scaleAdjustmentRepeatAttr ||
		plug == computeTwistAdjustmentsAttr ||
		plug == computeScaleAdjustmentsAttr ||
		plug == counterTwistBlendAttr ||
		plug == computeOrientationAttr ||
		plug == subdivisionsAttr ||
		plug == parameterizationAttr ||
		plug == outputCountAttr ||
		// This is the only draw attribute which effects the resulting computation data as we are optimizing its disabled state
		plug == drawRibbonAttr)
	{
		// Attribute has no affects relationship
		MDataBlock dataBlock = forceCache();
		dataBlock.outputValue(evalSinceDirtyAttr).setBool(false);

		MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
	}
	else if ( // These attributes do not need to force evaluation
		plug == customDrawSpaceTransformAttr ||
		plug == drawSpaceTransformationAttr ||
		plug == drawCurveAttr ||
		plug == drawNormalsAttr ||
		plug == drawHullAttr)
	{
		MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
	}

	return MStatus::kSuccess;
}

/*	Description
	-----------
	When the evaluation manager is in use, the draw state of the node is no longer dirtied
	Maya does not propagate dirtiness when using an evaluation manager
	We can use this callback to prepare the node for evaluation so that drawing will occur after compute has been run    */
MStatus FlexiChainDouble::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
{
	if (context.isNormal())
	{
		MStatus status;

		if ((evaluationNode.dirtyPlugExists(controlPoint0Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint0XAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint0YAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint0ZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint1Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint1XAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint1YAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint1ZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint2Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint2XAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint2YAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint2ZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint3Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint3XAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint3YAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(controlPoint3ZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorXAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorYAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(startTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(endTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(rollAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointOffset0Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointRadius0Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointVolume0Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointOffset1Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointRadius1Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointVolume1Attr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(normalUpVectorOverrideAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(normalUpVectorOverrideXAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(normalUpVectorOverrideYAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(normalUpVectorOverrideZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistUpVectorOverrideAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistUpVectorOverrideXAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistUpVectorOverrideYAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistUpVectorOverrideZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(normalUpVectorOverrideStateAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistUpVectorOverrideStateAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentCompoundAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentRampAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentRampPositionAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentRampValueAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentRampInterpolationAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentValueAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentOffsetAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentFalloffModeAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentFalloffDistanceAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(twistAdjustmentRepeatAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentCompoundAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentRampAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentRampPositionAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentRampValueAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentRampInterpolationAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentValueAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentValueXAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentValueYAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentValueZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentOffsetAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentFalloffModeAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentFalloffDistanceAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(scaleAdjustmentRepeatAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(computeTwistAdjustmentsAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(computeScaleAdjustmentsAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistBlendAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(computeOrientationAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(subdivisionsAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(parameterizationAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(outputCountAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(drawRibbonAttr, &status) && status))
		{
			MDataBlock dataBlock = forceCache();
			dataBlock.outputValue(evalSinceDirtyAttr).setBool(false);

			MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
		}
		else if (
			(evaluationNode.dirtyPlugExists(customDrawSpaceTransformAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(drawSpaceTransformationAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(drawCurveAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(drawNormalsAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(drawHullAttr, &status) && status))
		{
			MHWRender::MRenderer::setGeometryDrawDirty(thisMObject());
		}
	}

	return MStatus::kSuccess;
}

MStatus FlexiChainDouble::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputLocalFramesAttr && plug != outputLocalPositionsAttr && plug != outputLocalOrientationsAttr && plug != outputLocalScalesAttr &&
		plug != outputWorldFramesAttr && plug != outputWorldPositionsAttr && plug != outputWorldOrientationsAttr && plug != outputWorldScalesAttr)
		return MStatus::kUnknownParameter;

	// If evaluation has already occurred, this means a transformation is responsible for the current call, instead of data being pulled from a dirty input
	// This scenario can be optimised so that we only update the world space transformation for the cached local data
	if (!dataBlock.outputValue(evalSinceDirtyAttr).asBool())
		computeCurveData(dataBlock);

	if (plug == outputWorldFramesAttr)
	{
		// Frames must be transformed from local to world space
		MDagPath path;
		MDagPath::getAPathTo(thisMObject(), path);
		MMatrix worldTransform = path.inclusiveMatrix();

		MArrayDataHandle outFramesArrayHandle = dataBlock.outputArrayValue(outputWorldFramesAttr);
		// Connection functions should prevent multiple elements
		unsigned int elementCount = outFramesArrayHandle.elementCount();
		assert(elementCount <= 1);
		MArrayDataBuilder builder(&dataBlock, outputWorldFramesAttr, elementCount);

		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle outElementHandle = builder.addLast();
			MFnMatrixArrayData fnData;
			MMatrixArray outData;
			outData.setLength(m_data.outputCount);

			for (unsigned int j = 0; j < m_data.outputCount; j++)
				outData[j] = m_data.frames[j] * worldTransform;

			MObject outObj = fnData.create(outData);
			outElementHandle.setMObject(outObj);
		}

		outFramesArrayHandle.set(builder);
		outFramesArrayHandle.setAllClean();
	}
	else if (plug == outputWorldPositionsAttr)
	{
		MDagPath path;
		MDagPath::getAPathTo(thisMObject(), path);
		MMatrix worldTransform = path.inclusiveMatrix();

		MArrayDataHandle outPositionsArrayHandle = dataBlock.outputArrayValue(outputWorldPositionsAttr);
		unsigned int elementCount = outPositionsArrayHandle.elementCount();
		assert(elementCount <= 1);
		MArrayDataBuilder builder(&dataBlock, outputWorldPositionsAttr, elementCount);

		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle outElementHandle = builder.addLast();
			MFnVectorArrayData fnData;
			MVectorArray outData;
			outData.setLength(m_data.outputCount);

			for (unsigned int j = 0; j < m_data.outputCount; j++)
				outData[j] = MPoint{ m_data.frames[j][3] } *worldTransform;

			MObject outObj = fnData.create(outData);
			outElementHandle.setMObject(outObj);
		}

		outPositionsArrayHandle.set(builder);
		outPositionsArrayHandle.setAllClean();
	}
	else if (plug == outputWorldOrientationsAttr)
	{
		MDagPath path;
		MDagPath::getAPathTo(thisMObject(), path);
		MMatrix worldTransform = path.inclusiveMatrix();
		short rotateOrder = dataBlock.inputValue(rotateOrderAttr).asShort();

		MArrayDataHandle outOrientationArrayHandle = dataBlock.outputArrayValue(outputWorldOrientationsAttr);
		unsigned int elementCount = outOrientationArrayHandle.elementCount();
		assert(elementCount <= 1);
		MArrayDataBuilder builder(&dataBlock, outputWorldOrientationsAttr, elementCount);

		std::vector<MEulerRotation> outData;
		outData.resize(m_data.outputCount);

		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle outElementHandle = builder.addLast();
			MObject dataObj = outElementHandle.data();
			MFnPluginData fnData(dataObj);
			EulerArrayData* customData = (EulerArrayData*)fnData.data();

			for (unsigned int j = 0; j < m_data.outputCount; j++)
				outData[j] = MRS::extractEulerRotation(m_data.frames[j] * worldTransform, (MEulerRotation::RotationOrder)rotateOrder);

			customData->setArray(outData);
			outElementHandle.setMPxData((MPxData*)customData);
		}

		outOrientationArrayHandle.set(builder);
		outOrientationArrayHandle.setAllClean();
	}
	else if (plug == outputWorldScalesAttr)
	{
		MDagPath path;
		MDagPath::getAPathTo(thisMObject(), path);
		MMatrix worldTransform = path.inclusiveMatrix();
		MVector worldScale = MRS::extractScale(worldTransform);

		MArrayDataHandle outScalesArrayHandle = dataBlock.outputArrayValue(outputWorldScalesAttr);
		unsigned int elementCount = outScalesArrayHandle.elementCount();
		assert(elementCount <= 1);
		MArrayDataBuilder builder(&dataBlock, outputWorldScalesAttr, elementCount);

		for (unsigned int i = 0; i < elementCount; i++)
		{
			MDataHandle outElementHandle = builder.addLast();
			MFnVectorArrayData fnData;
			MVectorArray outData;
			outData.setLength(m_data.outputCount);

			for (unsigned int j = 0; j < m_data.outputCount; j++)
			{
				MVector localScale = MRS::extractScale(m_data.frames[j]);
				outData[j] = MVector{ localScale.x * worldScale.x, localScale.y * worldScale.y, localScale.z * worldScale.z };
			}

			MObject outObj = fnData.create(outData);
			outElementHandle.setMObject(outObj);
		}

		outScalesArrayHandle.set(builder);
		outScalesArrayHandle.setAllClean();
	}
	else if (plug == outputLocalFramesAttr)
	{
		outputMatrixDataArrayValue(dataBlock, outputLocalFramesAttr, m_data.frames);
	}
	else if (plug == outputLocalPositionsAttr)
	{
		std::vector<MVector> outData;
		outData.resize(m_data.outputCount);

		for (unsigned int i = 0; i < m_data.outputCount; i++)
			outData[i] = m_data.frames[i][3];

		outputVectorDataArrayValue(dataBlock, outputLocalPositionsAttr, outData);
	}
	else if (plug == outputLocalOrientationsAttr)
	{
		short rotateOrder = dataBlock.inputValue(rotateOrderAttr).asShort();

		std::vector<MEulerRotation> outData;
		outData.resize(m_data.outputCount);

		for (unsigned int i = 0; i < m_data.outputCount; i++)
			outData[i] = MRS::extractEulerRotation(m_data.frames[i], (MEulerRotation::RotationOrder)rotateOrder);

		outputPluginDataArrayValue<EulerArrayData, MEulerRotation>(dataBlock, outputLocalOrientationsAttr, outData);
	}
	else if (plug == outputLocalScalesAttr)
	{
		std::vector<MVector> outData;
		outData.resize(m_data.outputCount);

		for (unsigned int i = 0; i < m_data.outputCount; i++)
			outData[i] = MRS::extractScale(m_data.frames[i]);

		outputVectorDataArrayValue(dataBlock, outputLocalScalesAttr, outData);
	}

	return MStatus::kSuccess;
}

// ------ MPxLocatorNode ------

bool FlexiChainDouble::isBounded() const
{
	return true;
}

/*	Description
	-----------
	Beziers are contained within a convex hull defined by all of their control points    */
MBoundingBox FlexiChainDouble::boundingBox() const
{
	MBoundingBox bbox;

	for (unsigned int i = 0; i < m_data.controlPoints0.size(); i++)
		bbox.expand(m_data.controlPoints0[i]);
	for (unsigned int i = 0; i < m_data.controlPoints1.size(); i++)
		bbox.expand(m_data.controlPoints1[i]);

	return bbox;
}

// ------ Helpers ------

/*	Description
	------------
	This function is responsible for computing curve data based on the current input values
	The method is seperate from compute as our draw override needs to be able request updated data without cleaning the output attributes
	Basic state tracking has been implemented so that the draw cycle can query whether the function has already been invoked by MPxNode::compute()    */
void FlexiChainDouble::computeCurveData(MDataBlock& dataBlock)
{
	// --- Counts ---
	m_data.subdivisions = (unsigned)dataBlock.inputValue(subdivisionsAttr).asInt();
	m_data.outputCount = (unsigned)dataBlock.inputValue(outputCountAttr).asInt();
	m_data.parameterCount = m_data.outputCount + (m_data.outputCount - 1) * m_data.subdivisions;
	assert(m_data.parameterCount >= 2);

	// --- Shaping Parameters ---
	m_data.jointVolume0 = dataBlock.inputValue(jointVolume0Attr).asDouble();
	m_data.jointVolume1 = dataBlock.inputValue(jointVolume1Attr).asDouble();

	// --- Physical Points ---
	MVector P0 = dataBlock.inputValue(controlPoint0Attr).asVector();
	MVector P1 = dataBlock.inputValue(controlPoint1Attr).asVector();
	MVector P2 = dataBlock.inputValue(controlPoint2Attr).asVector();
	MVector P3 = dataBlock.inputValue(controlPoint3Attr).asVector();
	
	MVector vP1ToP0 = P0 - P1;
	vP1ToP0.normalize();
	MVector vP1ToP2 = P2 - P1;
	vP1ToP2.normalize();
	MVector vP0ToP2 = P2 - P0;
	vP0ToP2.normalize();
	MVector vP2ToP1 = P1 - P2;
	vP2ToP1.normalize();
	MVector vP2ToP3 = P3 - P2;
	vP2ToP3.normalize();
	MVector vP1ToP3 = P3 - P1;
	vP1ToP3.normalize();

	// --- Joint 0 ---
	// Rotate the P1ToP0 vector by offsetAngle0 (using Rodrigues formula) to find the offset direction of the first joint
	// We rotate the vector in both directions so that we can maximize the dot product delta of both results with the P1ToP2 vector
	double jointAngle0 = acos(vP1ToP0 * vP1ToP2);
	double offsetAngle0 = M_PI_2 - jointAngle0 / 2;
	double dot0 = vP1ToP0 * vP1ToP2;
	double cosOffsetAngle0 = cos(-offsetAngle0);
	double sinOffsetAngle0 = sin(-offsetAngle0);
	double cosCounterOffsetAngle0 = cos(offsetAngle0);
	double sinCounterOffsetAngle0 = sin(offsetAngle0);
	MVector vRotPlaneNormal0 = vP1ToP0 ^ vP1ToP2;
	vRotPlaneNormal0.normalize();
	MVector vCrossNormalWithP1ToP0 = vRotPlaneNormal0 ^ vP1ToP0;
	double dotNormalWithP1ToP0 = vRotPlaneNormal0 * vP1ToP0;
	MVector vRotatedP1ToP0 = vP1ToP0 * cosOffsetAngle0 + vCrossNormalWithP1ToP0 * sinOffsetAngle0 +
		vRotPlaneNormal0 * dotNormalWithP1ToP0 * (1.0 - cosOffsetAngle0);
	MVector vCounterRotatedP1ToP0 = vP1ToP0 * cosCounterOffsetAngle0 + vCrossNormalWithP1ToP0 * sinCounterOffsetAngle0 +
		vRotPlaneNormal0 * dotNormalWithP1ToP0 * (1.0 - cosCounterOffsetAngle0);

	// Find the maximum dot product delta and determine the offset directions
	MVector vLateralOffsetDirection0;
	MVector vPerpendicularOffsetDirection0;
	if (dot0 - vRotatedP1ToP0 * vP1ToP2 < dot0 - vCounterRotatedP1ToP0 * vP1ToP2)
	{
		vLateralOffsetDirection0 = vCounterRotatedP1ToP0;
		vPerpendicularOffsetDirection0 = vRotPlaneNormal0 ^ vRotatedP1ToP0;
	}
	else
	{
		vLateralOffsetDirection0 = vRotatedP1ToP0;
		vPerpendicularOffsetDirection0 = vRotatedP1ToP0 ^ vRotPlaneNormal0;
	}

	// Offset the mid point along the perpendicular vector
	// When the jointOffset attribute is set to a value of 1.0 the offset will roughly align the curve so that it passes through the mid point
	// Note, this is a very approximate calculation and is only true when the jointVolume attribute is also set to 1.0
	// To determine an exact value, we would need to sample the curve twice so that we could find the closest point on the curve without the offset applied
	double jointOffsetMultiplier0 = dataBlock.inputValue(jointOffset0Attr).asDouble();
	double theta0 = acos(vP1ToP0 * -vP0ToP2);
	double alpha0 = M_PI - (jointAngle0 / 2) - theta0;
	// We are finding the vector which goes from the mid point and intersects the startToEnd vector, then dividing by a constant to meet the above condition
	double jointOffset0 = ((P0 - P1).length() / sin(alpha0)) * sin(theta0) * jointOffsetMultiplier0 / 19.0;
	P1 = P1 + jointOffset0 * vPerpendicularOffsetDirection0;

	// --- Joint 1 ---
	// Rotate the P2ToP1 vector by offsetAngle1 (using Rodrigues formula) to find the offset direction of the second joint
	double jointAngle1 = acos(vP2ToP1 * vP2ToP3);
	double offsetAngle1 = M_PI_2 - jointAngle1 / 2;
	double dot1 = vP2ToP1 * vP2ToP3;
	double cosOffsetAngle1 = cos(-offsetAngle1);
	double sinOffsetAngle1 = sin(-offsetAngle1);
	double cosCounterOffsetAngle1 = cos(offsetAngle1);
	double sinCounterOffsetAngle1 = sin(offsetAngle1);
	MVector vRotPlaneNormal1 = vP2ToP1 ^ vP2ToP3;
	vRotPlaneNormal1.normalize();
	MVector vCrossNormalWithP2ToP1 = vRotPlaneNormal1 ^ vP2ToP1;
	double dotNormalWithP2ToP1 = vRotPlaneNormal1 * vP2ToP1;
	MVector vRotatedP2ToP1 = vP2ToP1 * cosOffsetAngle1 + vCrossNormalWithP2ToP1 * sinOffsetAngle1 +
		vRotPlaneNormal1 * dotNormalWithP2ToP1 * (1.0 - cosOffsetAngle1);
	MVector vCounterRotatedP2ToP1 = vP2ToP1 * cosCounterOffsetAngle1 + vCrossNormalWithP2ToP1 * sinCounterOffsetAngle1 +
		vRotPlaneNormal1 * dotNormalWithP2ToP1 * (1.0 - cosCounterOffsetAngle1);

	// Find the maximum dot product delta and determine the offset directions
	MVector vLateralOffsetDirection1;
	MVector vPerpendicularOffsetDirection1;
	if (dot1 - vRotatedP2ToP1 * vP2ToP3 < dot1 - vCounterRotatedP2ToP1 * vP2ToP3)
	{
		vLateralOffsetDirection1 = vCounterRotatedP2ToP1;
		vPerpendicularOffsetDirection1 = vRotPlaneNormal1 ^ vRotatedP2ToP1;
	}
	else
	{
		vLateralOffsetDirection1 = vRotatedP2ToP1;
		vPerpendicularOffsetDirection1 = vRotatedP2ToP1 ^ vRotPlaneNormal1;
	}

	// Offset the mid point along the perpendicular vector
	double jointOffsetMultiplier1 = dataBlock.inputValue(jointOffset1Attr).asDouble();
	double theta1 = acos(vP2ToP1 * -vP1ToP3);
	double alpha1 = M_PI - (jointAngle1 / 2) - theta1;
	double jointOffset1 = ((P1 - P2).length() / sin(alpha1)) * sin(theta1) * jointOffsetMultiplier1 / 19.0;
	P2 = P2 + jointOffset1 * vPerpendicularOffsetDirection1;

	// --- Virtual Points ---
	double jointRadius0 = dataBlock.inputValue(jointRadius0Attr).asDouble();
	double jointRadius1 = dataBlock.inputValue(jointRadius1Attr).asDouble();

	MVector vVirtualP0 = P0;
	MVector vVirtualP1 = P1 + vLateralOffsetDirection0 * jointRadius0;
	MVector vVirtualP2 = P1 + vLateralOffsetDirection0 * jointRadius0 * -1;
	MVector vVirtualP3 = P2 + vLateralOffsetDirection1 * jointRadius1;
	MVector vVirtualP4 = P2 + vLateralOffsetDirection1 * jointRadius1 * -1;
	MVector vVirtualP5 = P3;

	m_data.controlPoints0[0] = vVirtualP0;
	m_data.controlPoints0[1] = vVirtualP1;
	m_data.controlPoints0[2] = vVirtualP2;
	m_data.controlPoints0[3] = vVirtualP2 + (vVirtualP3 - vVirtualP2) / 2;
	m_data.controlPoints1[0] = m_data.controlPoints0[3];
	m_data.controlPoints1[1] = vVirtualP3;
	m_data.controlPoints1[2] = vVirtualP4;
	m_data.controlPoints1[3] = vVirtualP5;

	// --- Parameterization ---
	m_data.parameterization = dataBlock.inputValue(parameterizationAttr).asShort();

	// Could be optimized for when arc-length parameterization is disabled however this is rare, therefore simplify for the general case
	m_data.naturalPoints.resize(m_data.parameterCount);
	m_data.lengths.resize(m_data.parameterCount);
	m_data.naturalParameters.resize(m_data.parameterCount);
	m_data.arcLengthParameters.resize(m_data.parameterCount);
	m_data.splitLengthParameters.resize(m_data.parameterCount);

	for (unsigned int i = 0; i < m_data.parameterCount; ++i)
		m_data.naturalParameters[i] = (i * m_data.parameterRange) / (m_data.parameterCount - 1);

	// Arc-length parameterizations for this curve type are too specific to have a general implementation on the curve class
	computeLengths();
	computeArcLengthParameters();
	computeSplitLengthParameters();

	if (m_data.parameterization == 0)
		m_data.currentParameters = &m_data.naturalParameters;
	else if (m_data.parameterization == 1)
		m_data.currentParameters = &m_data.arcLengthParameters;
	else
		m_data.currentParameters = &m_data.splitLengthParameters;

	// --- Sample Curve ---
	m_data.isOrientEnabled = dataBlock.inputValue(computeOrientationAttr).asBool();
	m_data.sampleCount = m_data.isOrientEnabled ? m_data.parameterCount : m_data.outputCount;
	m_data.isNormalUpVectorOverrideEnabled = dataBlock.inputValue(normalUpVectorOverrideStateAttr).asBool();
	m_data.vNormalUp = m_data.isNormalUpVectorOverrideEnabled ? dataBlock.inputValue(normalUpVectorOverrideAttr).asVector()
		: dataBlock.inputValue(upVectorAttr).asVector();
	m_data.vNormalUp.normalize();
	m_data.isCounterTwistUpVectorOverrideEnabled = dataBlock.inputValue(counterTwistUpVectorOverrideStateAttr).asBool();
	m_data.vCounterTwistUp = m_data.isCounterTwistUpVectorOverrideEnabled ? dataBlock.inputValue(counterTwistUpVectorOverrideAttr).asVector()
		: dataBlock.inputValue(upVectorAttr).asVector();
	m_data.vCounterTwistUp.normalize();

	if (m_data.isOrientEnabled)
	{
		// RMF computation is iterative so we need to calculate a reflection for each parameter even if the frame count is small
		m_data.points.resize(m_data.sampleCount);
		m_data.tangents.resize(m_data.sampleCount);
		m_data.rmfReflections.resize(m_data.sampleCount);

		m_data.points[0] = sampleCurve((*m_data.currentParameters)[0]);
		m_data.tangents[0] = sampleFirstDerivative((*m_data.currentParameters)[0]);
		m_data.tangents[0].normalize();
		m_data.rmfReflections[0] = MQuaternion::identity;

		MVector vRight = m_data.vNormalUp ^ m_data.tangents[0];
		vRight.normalize();
		m_data.vPrincipalNormal = m_data.tangents[0] ^ vRight;
		MQuaternion qPrincipalNormal{ m_data.vPrincipalNormal.x, m_data.vPrincipalNormal.y, m_data.vPrincipalNormal.z, 0.0 };

		// RMF computation is iterative so we need to calculate an orientation for each parameter even if the frame count is small
		for (unsigned int i = 1; i < m_data.sampleCount; i++)
		{
			m_data.points[i] = sampleCurve((*m_data.currentParameters)[i]);
			m_data.tangents[i] = sampleFirstDerivative((*m_data.currentParameters)[i]);
			m_data.tangents[i].normalize();
			MQuaternion qReflection = m_curve.computeDoubleReflectionRMF(m_data.points[i - 1], m_data.points[i], m_data.tangents[i - 1], m_data.tangents[i]);
			m_data.rmfReflections[i] = MRS::quaternionMultiply(qReflection, m_data.rmfReflections[i - 1]);
		}

		// We are not optimizing the case where drawing of the ribbon is disabled (technically we could calculate normals at the specific output positions)
		m_data.normals.resize(m_data.sampleCount);
		m_data.binormals.resize(m_data.sampleCount);

		m_data.normals[0] = m_data.vPrincipalNormal;
		m_data.binormals[0] = m_data.tangents[0] ^ m_data.vPrincipalNormal;

		for (unsigned int i = 1; i < m_data.sampleCount; ++i)
		{
			MQuaternion qReflectionComposition = m_data.rmfReflections[i];
			MQuaternion qReflectionCompositionConjugate = qReflectionComposition.conjugate();
			MQuaternion qNormalProjected = MRS::quaternionMultiply(qReflectionComposition, qPrincipalNormal);
			MQuaternion qNormal = MRS::quaternionMultiply(qNormalProjected, qReflectionCompositionConjugate);
			MVector vNormal{ qNormal.x, qNormal.y, qNormal.z };
			vNormal.normalize();

			m_data.normals[i] = vNormal;
			m_data.binormals[i] = m_data.tangents[i] ^ vNormal;
		}
	}
	else
	{
		m_data.points.resize(m_data.sampleCount);

		for (unsigned int i = 0; i < m_data.sampleCount; i++)
		{
			unsigned int parameterIndex = i * (m_data.subdivisions + 1);
			m_data.points[i] = sampleCurve((*m_data.currentParameters)[parameterIndex]);
		}
	}

	// --- Scale Adjustments ---
	// Scale adjustments will be applied regardless of whether orientation is enabled
	m_data.isScaleAdjustmentEnabled = dataBlock.inputValue(computeScaleAdjustmentsAttr).asBool();
	if (m_data.isScaleAdjustmentEnabled)
		computeScaleAdjustments(dataBlock);

	// --- Twist Adjustments ---
	// Twist adjustments will only be applied when orientation is enabled
	m_data.isTwistAdjustmentEnabled = dataBlock.inputValue(computeTwistAdjustmentsAttr).asBool();
	if (m_data.isTwistAdjustmentEnabled && m_data.isOrientEnabled)
		computeTwistAdjustments(dataBlock);

	// --- Build Frames ---
	// Always keep frames in local space as this is required by draw
	// If compute needs world space transforms then it will be responsible for doing the conversions
	// If the counter-twist cache has been built, any twist generated by the moving RMF will be back-propagated down the curve
	m_data.frames.resize(m_data.outputCount);

	m_data.isDrawRibbonEnabled = dataBlock.inputValue(drawRibbonAttr).asBool();
	m_data.counterTwistBlend = dataBlock.inputValue(counterTwistBlendAttr).asDouble();
	m_data.counterTwist = dataBlock.inputValue(counterTwistAttr).asAngle().asRadians();
	m_data.startTwist = dataBlock.inputValue(startTwistAttr).asAngle().asRadians();
	m_data.endTwist = dataBlock.inputValue(endTwistAttr).asAngle().asRadians();
	m_data.roll = dataBlock.inputValue(rollAttr).asAngle().asRadians();

	if (m_data.isOrientEnabled)
	{
		unsigned int increment = m_data.isDrawRibbonEnabled ? 1 : m_data.subdivisions + 1;
		unsigned int outputIndex = 0;

		for (unsigned int i = 0; i < m_data.sampleCount; i += increment)
		{
			bool isOutput = !m_data.isDrawRibbonEnabled || i % (m_data.subdivisions + 1) == 0;
			double normalizedParam = m_data.naturalParameters[i] / m_data.parameterRange;
			double totalWeightedTwist = 0.0;

			// End twist
			double endTwistWeight = normalizedParam;
			double weightedEndTwist = m_data.endTwist * endTwistWeight;
			totalWeightedTwist += weightedEndTwist;

			// Start twist
			double startTwistWeight = 1 - endTwistWeight;
			double weightedStartTwist = m_data.startTwist * startTwistWeight;
			totalWeightedTwist += weightedStartTwist;

			// Counter twist
			double counterTwistWeight = -1 * endTwistWeight * m_data.counterTwistBlend;
			double weightedCounterTwist = m_data.counterTwist * counterTwistWeight;
			totalWeightedTwist += weightedCounterTwist;

			// Roll
			totalWeightedTwist += m_data.roll;

			// Twist adjustment
			if (m_data.isTwistAdjustmentEnabled)
			{
				for (TwistAdjustment& twistAdjustment : m_data.twistAdjustments)
				{
					// The falloff curve has a uniform parameter space and should be sampled using uniform inputs even if the curve is parameterized in terms of arc-length
					double twistAdjustmentWeight = twistAdjustment.curve.getValue(normalizedParam);
					double weightedTwistAdjustment = twistAdjustment.twist.asRadians() * twistAdjustmentWeight;
					totalWeightedTwist += weightedTwistAdjustment;
				}
			}

			// Scale adjustment
			MVector vScaleAdjustment{ 1.0, 1.0, 1.0 };
			if (m_data.isScaleAdjustmentEnabled)
			{
				for (ScaleAdjustment& scaleAdjustment : m_data.scaleAdjustments)
				{
					double scaleAdjustmentWeight = scaleAdjustment.curve.getValue(normalizedParam);
					MVector weightedScaleAdjustment = scaleAdjustment.vScale * scaleAdjustmentWeight;
					vScaleAdjustment += weightedScaleAdjustment;
				}
			}

			MRS::Matrix33<double> orientFrame{ m_data.tangents[i], m_data.normals[i], m_data.binormals[i] };
			orientFrame = orientFrame.preRotateInX(totalWeightedTwist);
			// Ensure scale is applied relative to all other transformations
			orientFrame = orientFrame.preScale(&vScaleAdjustment.x);

			// Update the caches so that draw has the current data
			orientFrame[0].get(m_data.tangents[i]);
			orientFrame[1].get(m_data.normals[i]);
			orientFrame[2].get(m_data.binormals[i]);

			if (isOutput)
			{
				MMatrix frame;
				orientFrame.get(frame);
				frame[3][0] = m_data.points[i].x; frame[3][1] = m_data.points[i].y; frame[3][2] = m_data.points[i].z;
				m_data.frames[outputIndex++] = frame;
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < m_data.sampleCount; i++)
		{
			MMatrix frame;
			unsigned int parameterIndex = i * (m_data.subdivisions + 1);
			double normalizedParam = m_data.naturalParameters[i] / m_data.parameterRange;

			// Scale adjustment
			if (m_data.isScaleAdjustmentEnabled)
			{
				MVector vScaleAdjustment{ 1.0, 1.0, 1.0 };
				for (ScaleAdjustment& scaleAdjustment : m_data.scaleAdjustments)
				{
					double scaleAdjustmentWeight = scaleAdjustment.curve.getValue(normalizedParam);
					MVector weightedScaleAdjustment = scaleAdjustment.vScale * scaleAdjustmentWeight;
					vScaleAdjustment += weightedScaleAdjustment;
				}

				frame[0][0] = vScaleAdjustment.x; frame[1][1] = vScaleAdjustment.y; frame[2][2] = vScaleAdjustment.z;
			}

			// Position
			frame[3][0] = m_data.points[i].x; frame[3][1] = m_data.points[i].y; frame[3][2] = m_data.points[i].z;
			m_data.frames[i] = frame;
		}
	}

	dataBlock.outputValue(evalSinceDirtyAttr).setBool(true);
	dataBlock.outputValue(drawSinceEvalAttr).setBool(false);
}

/*	Description
	-----------
	Function computes the scale adjustment data for the curve

	Considerations
	--------------
	It should only be necessary to call this function when scale adjustments are enabled
	- Care must then be taken to ensure scale adjustment data is only accessed when scale adjustments are enabled
	It has been separated from the compute function as the manipulators associated with this node also need to be able to trigger the update
	- When the associated array plug is altered (elements added or removed) this will not trigger a call to compute    */
void FlexiChainDouble::computeScaleAdjustments(MDataBlock& dataBlock)
{
	MArrayDataHandle scaleAdjustmentArrayHandle = dataBlock.inputArrayValue(scaleAdjustmentCompoundAttr);
	unsigned int numScaleAdjustments = scaleAdjustmentArrayHandle.elementCount();
	m_data.scaleAdjustments.resize(numScaleAdjustments);

	for (unsigned int i = 0; i < numScaleAdjustments; ++i)
	{
		MDataHandle scaleAdjustmentElementHandle = scaleAdjustmentArrayHandle.inputValue();

		// Get child handles for the current element
		MArrayDataHandle rampArrayHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentRampAttr);
		MDataHandle scaleHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentValueAttr);
		MDataHandle offsetHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentOffsetAttr);
		MDataHandle falloffDistanceHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentFalloffDistanceAttr);
		MDataHandle falloffModeHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentFalloffModeAttr);
		MDataHandle repeatHandle = scaleAdjustmentElementHandle.child(scaleAdjustmentRepeatAttr);

		// Retrieve data for current adjustment and store in struct
		ScaleAdjustment scaleAdjustment;
		scaleAdjustment.vScale = scaleHandle.asVector();
		scaleAdjustment.offset = offsetHandle.asDouble();
		scaleAdjustment.falloffDistance = falloffDistanceHandle.asDouble();
		scaleAdjustment.falloffMode = falloffModeHandle.asShort();
		scaleAdjustment.repeat = repeatHandle.asBool();

		// Curve will be formed by ramp control points
		unsigned int numRampControlPoints = rampArrayHandle.elementCount();
		unsigned int numTotalControlPoints = scaleAdjustment.falloffMode == 2 ? numRampControlPoints * 2 : numRampControlPoints;
		// Note, we cannot resize the controlPoints vector as the CV class does not have a default constructor (reserve instead)
		scaleAdjustment.controlPoints.reserve(numTotalControlPoints);

		for (unsigned int j = 0; j < numRampControlPoints; ++j)
		{
			MDataHandle controlPointHandle = rampArrayHandle.inputValue();
			float position = controlPointHandle.child(scaleAdjustmentRampPositionAttr).asFloat();
			position = std::max(std::min(position, 1.0f), 0.0f);
			float value = controlPointHandle.child(scaleAdjustmentRampValueAttr).asFloat();
			value = std::max(std::min(value, 1.0f), 0.0f);
			short interp = controlPointHandle.child(scaleAdjustmentRampInterpolationAttr).asShort();

			// Forward
			if (scaleAdjustment.falloffMode == 0)
			{
				double adjustedPosition = (double)position * scaleAdjustment.falloffDistance + scaleAdjustment.offset;
				scaleAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Backward
			else if (scaleAdjustment.falloffMode == 1)
			{
				double adjustedPosition = (double)position * -1 * scaleAdjustment.falloffDistance + scaleAdjustment.offset;
				scaleAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Mirror
			else if (scaleAdjustment.falloffMode == 2)
			{
				// The entire mirrored curve has a total default falloff distance of 1.0
				double forwardPosition = (double)position * 0.5 * scaleAdjustment.falloffDistance + scaleAdjustment.offset;
				double backwardPosition = (double)position * -0.5 * scaleAdjustment.falloffDistance + scaleAdjustment.offset;
				scaleAdjustment.controlPoints.push_back({ backwardPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
				scaleAdjustment.controlPoints.push_back({ forwardPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Center
			else if (scaleAdjustment.falloffMode == 3)
			{
				// Remap: [0, 1] -> [-0.5, 0.5] = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
				double remappedPosition = -0.5 + (double)position;
				double adjustedPosition = remappedPosition * scaleAdjustment.falloffDistance + scaleAdjustment.offset;
				scaleAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}

			rampArrayHandle.next();
		}

		// If mirroring has occurred, the current interpolation type of each mirrored segment is currently incorrect
		// This is because the interpolation type of an entire segment is defined by its lower bound control point
		// Currently, each mirrored segment has a lower bound control point whose interpolation type is defined by an upper bound control point
		std::sort(scaleAdjustment.controlPoints.begin(), scaleAdjustment.controlPoints.end(), SeExpr2::Curve<double>::cvLessThan);
		if (scaleAdjustment.falloffMode == 1 || scaleAdjustment.falloffMode == 2)
		{
			auto cvIter = scaleAdjustment.controlPoints.begin();
			while (cvIter != scaleAdjustment.controlPoints.end() && cvIter->_pos < scaleAdjustment.offset)
			{
				cvIter->_interp = std::next(cvIter)->_interp;
				cvIter++;
			}
		}

		// If repeat is enabled, ensure the adjustment has full coverage over the curve's domain
		if (scaleAdjustment.repeat && numRampControlPoints >= 2)
		{
			double lowerBoundPos = scaleAdjustment.controlPoints[0]._pos;
			double upperBoundPos = scaleAdjustment.controlPoints[scaleAdjustment.controlPoints.size() - 1]._pos;

			// The adjustment's complexity has an inversely proportional relationship to the falloff distance, implement a limit
			if (scaleAdjustment.falloffDistance > 0.01)
			{
				unsigned int lowerRepititionCount = std::max(0, (int)std::ceil(lowerBoundPos / scaleAdjustment.falloffDistance));
				unsigned int upperRepititionCount = std::max(0, (int)std::ceil((1.0 - upperBoundPos) / scaleAdjustment.falloffDistance));
				unsigned int totalRepitionCount = lowerRepititionCount + upperRepititionCount;

				scaleAdjustment.controlPoints.reserve(numTotalControlPoints * (totalRepitionCount + 1));

				for (unsigned int j = 0; j < lowerRepititionCount; ++j)
				{
					for (unsigned int k = 0; k < numTotalControlPoints - 1; ++k)
					{
						scaleAdjustment.controlPoints.push_back(scaleAdjustment.controlPoints[k]);
						scaleAdjustment.controlPoints.back()._pos -= scaleAdjustment.falloffDistance * (j + 1);
					}

					// We must ensure the last control point of the current set is not overlapping the first control point of the previous set
					scaleAdjustment.controlPoints.push_back(scaleAdjustment.controlPoints[numTotalControlPoints - 1]);
					scaleAdjustment.controlPoints.back()._pos -= scaleAdjustment.falloffDistance * (j + 1) + 0.00001;
				}

				for (unsigned int j = 0; j < upperRepititionCount; ++j)
				{
					// We must ensure the first control point of the current set is not overlapping the last control point of the previous set
					scaleAdjustment.controlPoints.push_back(scaleAdjustment.controlPoints[0]);
					scaleAdjustment.controlPoints.back()._pos += scaleAdjustment.falloffDistance * (j + 1) + 0.00001;

					for (unsigned int k = 1; k < numTotalControlPoints; ++k)
					{
						scaleAdjustment.controlPoints.push_back(scaleAdjustment.controlPoints[k]);
						scaleAdjustment.controlPoints.back()._pos += scaleAdjustment.falloffDistance * (j + 1);
					}
				}

				// Override the interpolation type of the last control point of each repition so that we get a jump in value between sets
				for (unsigned int j = numTotalControlPoints - 1; j < scaleAdjustment.controlPoints.size(); j += numTotalControlPoints)
					scaleAdjustment.controlPoints[j]._interp = SeExpr2::Curve<double>::InterpType::kNone;
			}
		}

		for (SeExpr2::Curve<double>::CV& controlPoint : scaleAdjustment.controlPoints)
			scaleAdjustment.curve.addPoint(controlPoint._pos, controlPoint._val, controlPoint._interp);

		// This will again sort the control points by position (Maya's ramps do not keep element plugs ordered)
		// It also creates a linear segment at each boundary of the curve where the value of each segment matches the first and last control points respectively
		scaleAdjustment.curve.preparePoints();
		m_data.scaleAdjustments[i] = scaleAdjustment;

		scaleAdjustmentArrayHandle.next();
	}
}

/*	Description
	-----------
	Function computes the twist adjustment data for the curve

	Considerations
	--------------
	It should only be necessary to call this function when twist adjustments and orientation computation are enabled
	- Care must then be taken to ensure twist adjustment data is only accessed when twist adjustments and orientation computation are enabled
	It has been separated from the compute function as the manipulators associated with this node also need to be able to trigger the update
	- When the associated array plug is altered (elements added or removed) this will not trigger a call to compute    */
void FlexiChainDouble::computeTwistAdjustments(MDataBlock& dataBlock)
{
	MArrayDataHandle twistAdjustmentArrayHandle = dataBlock.inputArrayValue(twistAdjustmentCompoundAttr);
	unsigned int numTwistAdjustments = twistAdjustmentArrayHandle.elementCount();
	m_data.twistAdjustments.resize(numTwistAdjustments);

	for (unsigned int i = 0; i < numTwistAdjustments; ++i)
	{
		MDataHandle twistAdjustmentElementHandle = twistAdjustmentArrayHandle.inputValue();

		// Get child handles for the current element
		MArrayDataHandle rampArrayHandle = twistAdjustmentElementHandle.child(twistAdjustmentRampAttr);
		MDataHandle twistHandle = twistAdjustmentElementHandle.child(twistAdjustmentValueAttr);
		MDataHandle offsetHandle = twistAdjustmentElementHandle.child(twistAdjustmentOffsetAttr);
		MDataHandle falloffDistanceHandle = twistAdjustmentElementHandle.child(twistAdjustmentFalloffDistanceAttr);
		MDataHandle falloffModeHandle = twistAdjustmentElementHandle.child(twistAdjustmentFalloffModeAttr);
		MDataHandle repeatHandle = twistAdjustmentElementHandle.child(twistAdjustmentRepeatAttr);

		// Retrieve data for current adjustment and store in struct
		TwistAdjustment twistAdjustment;
		twistAdjustment.twist = twistHandle.asAngle();
		twistAdjustment.offset = offsetHandle.asDouble();
		twistAdjustment.falloffDistance = falloffDistanceHandle.asDouble();
		twistAdjustment.falloffMode = falloffModeHandle.asShort();
		twistAdjustment.repeat = repeatHandle.asBool();

		// Curve will be formed by ramp control points
		unsigned int numRampControlPoints = rampArrayHandle.elementCount();
		unsigned int numTotalControlPoints = twistAdjustment.falloffMode == 2 ? numRampControlPoints * 2 : numRampControlPoints;
		// Note, we cannot resize the controlPoints vector as the CV class does not have a default constructor (reserve instead)
		twistAdjustment.controlPoints.reserve(numTotalControlPoints);

		for (unsigned int j = 0; j < numRampControlPoints; ++j)
		{
			MDataHandle controlPointHandle = rampArrayHandle.inputValue();
			float position = controlPointHandle.child(twistAdjustmentRampPositionAttr).asFloat();
			position = std::max(std::min(position, 1.0f), 0.0f);
			float value = controlPointHandle.child(twistAdjustmentRampValueAttr).asFloat();
			value = std::max(std::min(value, 1.0f), 0.0f);
			short interp = controlPointHandle.child(twistAdjustmentRampInterpolationAttr).asShort();

			// Forward
			if (twistAdjustment.falloffMode == 0)
			{
				double adjustedPosition = (double)position * twistAdjustment.falloffDistance + twistAdjustment.offset;
				twistAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Backward
			else if (twistAdjustment.falloffMode == 1)
			{
				double adjustedPosition = (double)position * -1 * twistAdjustment.falloffDistance + twistAdjustment.offset;
				twistAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Mirror
			else if (twistAdjustment.falloffMode == 2)
			{
				// The entire mirrored curve has a total default falloff distance of 1.0
				double forwardPosition = (double)position * 0.5 * twistAdjustment.falloffDistance + twistAdjustment.offset;
				double backwardPosition = (double)position * -0.5 * twistAdjustment.falloffDistance + twistAdjustment.offset;
				twistAdjustment.controlPoints.push_back({ backwardPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
				twistAdjustment.controlPoints.push_back({ forwardPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}
			// Center
			else if (twistAdjustment.falloffMode == 3)
			{
				// Remap: [0, 1] -> [-0.5, 0.5] = low2 + (value - low1) * (high2 - low2) / (high1 - low1)
				double remappedPosition = -0.5 + (double)position;
				double adjustedPosition = remappedPosition * twistAdjustment.falloffDistance + twistAdjustment.offset;
				twistAdjustment.controlPoints.push_back({ adjustedPosition, (double)value, static_cast<SeExpr2::Curve<double>::InterpType>(interp) });
			}

			rampArrayHandle.next();
		}

		// If mirroring has occurred, the current interpolation type of each mirrored segment is currently incorrect
		// This is because the interpolation type of an entire segment is defined by its lower bound control point
		// Currently, each mirrored segment has a lower bound control point whose interpolation type is defined by an upper bound control point
		if (twistAdjustment.falloffMode == 1 || twistAdjustment.falloffMode == 2)
		{
			std::sort(twistAdjustment.controlPoints.begin(), twistAdjustment.controlPoints.end(), SeExpr2::Curve<double>::cvLessThan);
			auto cvIter = twistAdjustment.controlPoints.begin();
			while (cvIter != twistAdjustment.controlPoints.end() && cvIter->_pos < twistAdjustment.offset)
			{
				cvIter->_interp = std::next(cvIter)->_interp;
				cvIter++;
			}
		}

		// If repeat is enabled, ensure the adjustment has full coverage over the curve's domain
		if (twistAdjustment.repeat && numRampControlPoints >= 2)
		{
			double lowerBoundPos = twistAdjustment.controlPoints[0]._pos;
			double upperBoundPos = twistAdjustment.controlPoints[twistAdjustment.controlPoints.size() - 1]._pos;

			// The adjustment's complexity has an inversely proportional relationship to the falloff distance, implement a limit
			if (twistAdjustment.falloffDistance > 0.01)
			{
				unsigned int lowerRepititionCount = std::max(0, (int)std::ceil(lowerBoundPos / twistAdjustment.falloffDistance));
				unsigned int upperRepititionCount = std::max(0, (int)std::ceil((1.0 - upperBoundPos) / twistAdjustment.falloffDistance));
				unsigned int totalRepitionCount = lowerRepititionCount + upperRepititionCount;

				twistAdjustment.controlPoints.reserve(numTotalControlPoints * (totalRepitionCount + 1));

				for (unsigned int j = 0; j < lowerRepititionCount; ++j)
				{
					for (unsigned int k = 0; k < numTotalControlPoints - 1; ++k)
					{
						twistAdjustment.controlPoints.push_back(twistAdjustment.controlPoints[k]);
						twistAdjustment.controlPoints.back()._pos -= twistAdjustment.falloffDistance * (j + 1);
					}

					// We must ensure the last control point of the current set is not overlapping the first control point of the previous set
					twistAdjustment.controlPoints.push_back(twistAdjustment.controlPoints[numTotalControlPoints - 1]);
					twistAdjustment.controlPoints.back()._pos -= twistAdjustment.falloffDistance * (j + 1) + 0.00001;
				}

				for (unsigned int j = 0; j < upperRepititionCount; ++j)
				{
					// We must ensure the first control point of the current set is not overlapping the last control point of the previous set
					twistAdjustment.controlPoints.push_back(twistAdjustment.controlPoints[0]);
					twistAdjustment.controlPoints.back()._pos += twistAdjustment.falloffDistance * (j + 1) + 0.00001;

					for (unsigned int k = 1; k < numTotalControlPoints; ++k)
					{
						twistAdjustment.controlPoints.push_back(twistAdjustment.controlPoints[k]);
						twistAdjustment.controlPoints.back()._pos += twistAdjustment.falloffDistance * (j + 1);
					}
				}

				// Override the interpolation type of the last control point of each repition so that we get a jump in value between sets
				for (unsigned int j = numTotalControlPoints - 1; j < twistAdjustment.controlPoints.size(); j += numTotalControlPoints)
					twistAdjustment.controlPoints[j]._interp = SeExpr2::Curve<double>::InterpType::kNone;
			}
		}

		for (SeExpr2::Curve<double>::CV& controlPoint : twistAdjustment.controlPoints)
			twistAdjustment.curve.addPoint(controlPoint._pos, controlPoint._val, controlPoint._interp);

		// This will again sort the control points by position (Maya's ramps do not keep element plugs ordered)
		// It also creates a linear segment at each boundary of the curve where the value of each segment matches the first and last control points respectively
		twistAdjustment.curve.preparePoints();
		m_data.twistAdjustments[i] = twistAdjustment;

		twistAdjustmentArrayHandle.next();
	}
}

/*	Description
	-----------
	Produces an array of lengths (of the current size), each corresponding to a uniform increment of the natural parameter
	Also produces an array of points sampled at each of the natural parameters
	- These values are used as the basis for approximating the below parameterization functions
	- Used when a mapping between arc-length values and natural parameters is required    */
void FlexiChainDouble::computeLengths()
{
	// Cache arc-lengths by uniformly incrementing over the curves natural parameter t
	// In this case the combined curve P(t) has a domain P:[0,2] as we are incrementing over two continuous curves
	m_data.naturalPoints[0] = sampleCurve(0.0);
	m_data.lengths[0] = 0.0;

	for (unsigned int i = 1; i < m_data.parameterCount; i++)
	{
		m_data.naturalPoints[i] = sampleCurve(m_data.naturalParameters[i]);
		m_data.lengths[i] = m_data.lengths[i - 1] + (m_data.naturalPoints[i - 1] - m_data.naturalPoints[i]).length();
	}
}

/*	Description
	-----------
	Produces an array of values for the natural parameter t, each corresponding to a uniform increment of the arc-length

	The curve can be parameterized in terms of its arc-length such that it has unit parametric speed (uniform motion)
	Consider s to be the normalized arc-length of the curve for some value of t
	There is an increasing relationship between s and t over the curve's domain [0,2]
	This relationship can be expressed as L(t)=s, L:[0,2]->[0,1]
	Subsequently the relationship can be expressed as L^-1(s)=t, L^-1:[0,1]->[0,2]
	The parameterization of the curve P(t) in terms of its arc-length can be expressed as a composite function Q(s) = P(L^-1(s))
	It is very difficult to find a closed-form solution to L^-1 however it can be approximated numerically    */
void FlexiChainDouble::computeArcLengthParameters()
{
	// Using the total arc-length, iterate over uniform increments and approximate values of t
	// We are essentially solving the function L^-1(s) described above, however our values for s are not normalized (the concept remains the same)
	// We don't want to approximate the end parameters of the curve as they should always be 0 and 3
	m_data.arcLengthParameters[0] = 0.0;
	double totalLength = m_data.lengths[m_data.parameterCount - 1];
	double paramMult = 1.0 / m_data.parameterRange;

	// The number of iterations accounts for the two end parameters
	for (unsigned int i = 1; i < m_data.parameterCount - 1; i++)
	{
		double targetLength = m_data.naturalParameters[i] * totalLength * paramMult;

		// Complete a binary search to find the closest tLength which is smaller than the targetLength
		// We then just need to retrieve the t value which was used to calulate the tLength
		unsigned int low = 0;
		unsigned int high = m_data.parameterCount;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = m_data.lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		// The last iteration of the search may cause the index to increase due to the low value increasing on the second last iteration
		// In this case, the tLength at the current index may be larger than the targetLength
		// We must complete one final check and decrement the index if this was the case
		if (tLength > targetLength)
			index -= 1;

		assert(m_data.lengths[index] <= targetLength);
		assert(m_data.lengths[index + 1] > targetLength);

		// To retrieve the t value for the tLength we can calculate, t = index / ((parameterCount - 1) * paramMult)
		// However, we can increase the accuracy by determining the skew of the target length in relation to the lengths before and after
		double tLengthBefore = m_data.lengths[index];
		double tLengthAfter = m_data.lengths[index + 1];
		double t = (index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((m_data.parameterCount - 1) * paramMult);
		m_data.arcLengthParameters[i] = t;
	}

	m_data.arcLengthParameters[m_data.parameterCount - 1] = m_data.parameterRange;
}

/*	Description
	-----------
	Produces an array of values for the natural parameter t, designed to stabalize both joints
	It provides the ability to stretch the chain whilst maintaining the integrity of its initial form

	The calculation splits the curve into three segments determined by two points on the curve
	- One closest to the point half way between virtual control points P1 and P2 and one closest to the point half way between virtual control points P3 and P4
	We want to parameterize each segment of the curve in terms of its arc-length, such that each segment has unit parametric speed (uniform motion)
	- There will be an instantaneous change in velocity at the points where two segments meet
	- Each segment should be responsible for calculating a third of the parameters    */
void FlexiChainDouble::computeSplitLengthParameters()
{
	double paramMult = 1.0 / m_data.parameterRange;

	// The curve is not well defined if there are less than four samples
	m_data.splitLengthParameters[0] = 0.0;
	if (m_data.parameterCount == 2)
	{
		m_data.splitLengthParameters[1] = 2.0;
		return;
	}
	else if (m_data.parameterCount == 3)
	{
		m_data.splitLengthParameters[1] = 1.0;
		m_data.splitLengthParameters[2] = 2.0;
		return;
	}

	// We can approximate the two stable parameters by minimizing the distance to our target points
	// We could also minimize the dot product of the displacement vector with the tangent vector: (p - q(t)) . q'(t) = 0
	// However the latter solution becomes unstable when a small sample count is used and is more compuationally heavy (requires derivative at each point)
	// We know each of the closest points will occur close to 25% and 75% of the curves natural parameter respectively so we will begin our search from here
	// This also avoids the possibility of finding a local minimum instead of the global minimum for each point which is what we are looking for

	// --- Closest Point Joint0 ---
	MPoint targetPoint = m_data.controlPoints0[1] + (m_data.controlPoints0[2] - m_data.controlPoints0[1]) / 2;
	// This will always be larger than 0 and smaller than parameterCount-1
	unsigned int closestIndex0 = m_data.parameterCount / 4;
	MVector vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0];
	double closestMag = vDisplacement.length();
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 - 1];
	double earlyMag = vDisplacement.length();
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + 1];
	double lateMag = vDisplacement.length();

	char direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex0 += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex0 == 0 && direction == -1)
				break;
			if (closestIndex0 == m_data.parameterCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + direction];
			nextMag = vDisplacement.length();
		}
	}

	// We can calculate, tStable0 = closestIndex0 / ((parameterCount - 1) * paramMult)
	// However, we can increase the accuracy by determining the skew of the current dot product in relation to the dot product before or after
	// We are determining if the minimum occurs before or after the current index by querying whether the sign of the dot product calculations changes
	// If the sign does not change then the minimum is not skewed, indicating the closest index is one of the end points
	double tStable0 = closestIndex0 / ((m_data.parameterCount - 1) * 0.5);

	double earlyProduct;
	double lateProduct;
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0]; vDisplacement.normalize();
	MVector vTangent = sampleFirstDerivative(closestIndex0 / ((m_data.parameterCount - 1) * paramMult));
	double closestProduct = vDisplacement * vTangent;

	if (closestIndex0 == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex0 - 1) / ((m_data.parameterCount - 1) * paramMult));
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex0 == m_data.parameterCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex0 + 1) / ((m_data.parameterCount - 1) * paramMult));
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly0 = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate0 = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);

	if (isMinimumEarly0)
	{
		// The minimum occurs before tStable0
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable0 = tStable0 - step * minimumRatio;
	}
	else if (isMinimumLate0)
	{
		// The minimum occurs after tStable0
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable0 = tStable0 + step * minimumRatio;
	}

	// --- Closest Point Joint1 ---
	targetPoint = m_data.controlPoints1[1] + (m_data.controlPoints1[2] - m_data.controlPoints1[1]) / 2;
	// This will always be larger than 0 but may be equal to parameterCount-1
	unsigned int closestIndex1 = (m_data.parameterCount * 3) / 4;
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1];
	closestMag = vDisplacement.length();
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 - 1];
	earlyMag = vDisplacement.length();

	if (closestIndex1 == m_data.parameterCount - 1)
		lateMag = closestMag;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + 1];
		lateMag = vDisplacement.length();
	}

	direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex1 += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex1 == 0 && direction == -1)
				break;
			if (closestIndex1 == m_data.parameterCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + direction];
			nextMag = vDisplacement.length();
		}
	}

	// Same as before, determine the skew
	double tStable1 = closestIndex1 / ((m_data.parameterCount - 1) * paramMult);
	
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1]; vDisplacement.normalize();
	vTangent = sampleFirstDerivative(closestIndex1 / ((m_data.parameterCount - 1) * paramMult));
	closestProduct = vDisplacement * vTangent;

	if (closestIndex1 == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex1 - 1) / ((m_data.parameterCount - 1) * paramMult));
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex1 == m_data.parameterCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex1 + 1) / ((m_data.parameterCount - 1) * paramMult));
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly1 = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate1 = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);
	
	if (isMinimumEarly1)
	{
		// The minimum occurs before tStable1
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable1 = tStable1 - step * minimumRatio;
	}
	else if (isMinimumLate1)
	{
		// The minimum occurs after tStable1
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable1 = tStable1 + step * minimumRatio;
	}

	// --- Edge Cases ---
	// When both joints overlap each other, the entire second segment of the curve becomes a minimum
	// If this occurs we will ensure all of the samples for the second segment are condensed to this single point
	if (tStable0 > tStable1)
	{
		tStable0 = tStable1;
		closestIndex0 = closestIndex1;
		isMinimumEarly0 = isMinimumEarly1;
		isMinimumLate0 = isMinimumLate1;
	}

	// --- Sample Distribution ---
	// When parameterizing the curve, we attempt to evenly distribute samples between each segment
	// In the case where the parameterCount is not evenly divisible by the 3 segments, the first segments will have preference and receive the extra samples
	// There are four constant points (start, stable0, stable1, end), from this we can determine the distribution:
	unsigned int remainingCount = m_data.parameterCount - 4;
	unsigned int segment0SampleCount = (unsigned)std::ceil(remainingCount / 3.0);
	remainingCount -= segment0SampleCount;
	unsigned int segment1SampleCount = (unsigned)std::ceil(remainingCount / 2.0);
	remainingCount -= segment1SampleCount;
	unsigned int segment2SampleCount = remainingCount;

	unsigned int stable0Index = segment0SampleCount + 1; // +1 for start point
	unsigned int stable1Index = segment0SampleCount + segment1SampleCount + 2; // +2 for start and stable0 points

	// --- Segment0 ---
	// Using the total arc-length of segment0, iterate over uniform increments and approximate values of t for (0, tStable0)
	double segment0ArcLength = m_curve.naturalToArcLengthParameter(tStable0 * paramMult, m_data.lengths) * m_data.lengths[m_data.parameterCount - 1];
	double segment0Step = 1.0 / (segment0SampleCount + 1);
	double segment0Percent = 0.0;

	for (unsigned int i = 1; i < stable0Index; ++i)
	{
		segment0Percent += segment0Step;
		double targetLength = segment0Percent * segment0ArcLength;

		// Complete a binary search to find the closest tLength which is smaller than the targetLength
		// We then just need to retrieve the t value which was used to calulate the tLength
		unsigned int low = 0;
		unsigned int high = isMinimumLate0 ? closestIndex0 + 1 : closestIndex0;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = m_data.lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		// The last iteration of the search may cause the index to increase due to the low value increasing on the second last iteration
		// In this case, the tLength at the current index may be larger than the targetLength
		// We must complete one final check and decrement the index if this was the case
		if (tLength > targetLength)
			index -= 1;

		assert(m_data.lengths[index] <= targetLength);
		assert(m_data.lengths[index + 1] > targetLength);

		// To retrieve the t value for the tLength we can calculate, t = index / ((parameterCount - 1) * paramMult)
		// However, we can increase the accuracy by determining the skew of the target length in relation to the lengths before and after
		double tLengthBefore = m_data.lengths[index];
		double tLengthAfter = m_data.lengths[index + 1];
		double t = (index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((m_data.parameterCount - 1) * 0.5);
		m_data.splitLengthParameters[i] = t;
	}

	m_data.splitLengthParameters[stable0Index] = tStable0;

	// --- Segment1 ---
	// Using the total arc-length of segment1, iterate over uniform increments and approximate values of t for (tStable0, tStable1)
	double segment1ArcLength = m_curve.naturalToArcLengthParameter(tStable1 * paramMult, m_data.lengths) * 
		m_data.lengths[m_data.parameterCount - 1] - segment0ArcLength;
	double segment1Step = 1.0 / (segment1SampleCount + 1);
	double segment1Percent = 0.0;

	for (unsigned int i = stable0Index + 1; i < stable1Index; ++i)
	{
		segment1Percent += segment1Step;
		double targetLength = segment0ArcLength + segment1Percent * segment1ArcLength;

		unsigned int low = isMinimumEarly0 ? closestIndex0 - 1 : closestIndex0;
		unsigned int high = isMinimumLate1 ? closestIndex1 + 1 : closestIndex1;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = m_data.lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		if (tLength > targetLength)
			index -= 1;

		assert(m_data.lengths[index] <= targetLength);
		assert(m_data.lengths[index + 1] > targetLength);

		double tLengthBefore = m_data.lengths[index];
		double tLengthAfter = m_data.lengths[index + 1];
		double t = (index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((m_data.parameterCount - 1) * paramMult);
		m_data.splitLengthParameters[i] = t;
	}

	m_data.splitLengthParameters[stable1Index] = tStable1;

	// --- Segment2 ---
	// Using the total arc-length of segment2, iterate over uniform increments and approximate values of t for (tStable1, 2.0)
	double segment2ArcLength = m_data.lengths[m_data.parameterCount - 1] - segment0ArcLength - segment1ArcLength;
	double segment2Step = 1.0 / (segment2SampleCount + 1);
	double segment2Percent = 0.0;

	for (unsigned int i = stable1Index + 1; i < m_data.parameterCount - 1; ++i)
	{
		segment2Percent += segment2Step;
		double targetLength = segment0ArcLength + segment1ArcLength + segment2Percent * segment2ArcLength;

		unsigned int low = isMinimumEarly1 ? closestIndex1 - 1 : closestIndex1;
		unsigned int high = m_data.parameterCount;
		unsigned int index;
		double tLength;

		while (low < high)
		{
			index = low + (high - low) / 2;
			tLength = m_data.lengths[index];

			if (tLength < targetLength)
				low = index + 1;
			else
				high = index;
		}

		if (tLength > targetLength)
			index -= 1;

		assert(m_data.lengths[index] <= targetLength);
		assert(m_data.lengths[index + 1] > targetLength);

		double tLengthBefore = m_data.lengths[index];
		double tLengthAfter = m_data.lengths[index + 1];
		double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((m_data.parameterCount - 1) * paramMult);
		m_data.splitLengthParameters[i] = t;
	}

	m_data.splitLengthParameters[m_data.parameterCount - 1] = m_data.parameterRange;
}

/*	Description
	-----------
	Calculates an equivalent natural parameter approximation for the given split-length parameter
	We will calculate the stable parameters from which we can determine the arc-lengths of each segment
	We can then determine the arc-length at the given parameter by determining its proportional position within the segment's parameterized range
	From the arc-length we can determine the natural parameter

	Args
	----
	splitLengthParameter = The split-length parameter from which to calculate the equivalent natural parameter, range = [0.0, 2.0]    */
double FlexiChainDouble::splitLengthToNaturalParameter(double splitLengthParameter)
{
	if (splitLengthParameter == 0.0 || splitLengthParameter == 2.0)
		return splitLengthParameter;

	double paramMult = 1.0 / m_data.parameterRange;

	// --- Closest Point Joint0 ---
	MPoint targetPoint = m_data.controlPoints0[1] + (m_data.controlPoints0[2] - m_data.controlPoints0[1]) / 2;
	// This may be equal to 0 but will always be smaller than parameterCount-1
	unsigned int closestIndex0 = m_data.parameterCount / 4;
	MVector vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0];
	double closestMag = vDisplacement.length();

	double earlyMag;
	if (closestIndex0 == 0)
		earlyMag = closestMag;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 - 1];
		earlyMag = vDisplacement.length();
	}

	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + 1];
	double lateMag = vDisplacement.length();

	char direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex0 += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex0 == 0 && direction == -1)
				break;
			if (closestIndex0 == m_data.parameterCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + direction];
			nextMag = vDisplacement.length();
		}
	}

	// We can calculate, tStable0 = closestIndex0 / ((parameterCount - 1) * paramMult)
	// However, we can increase the accuracy by determining the skew of the current dot product in relation to the dot product before or after
	// We are determining if the minimum occurs before or after the current index by querying whether the sign of the dot product calculations changes
	// If the sign does not change then the minimum is not skewed, indicating the closest index is one of the end points
	double tStable0 = closestIndex0 / ((m_data.parameterCount - 1) * paramMult);

	double earlyProduct;
	double lateProduct;
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0]; vDisplacement.normalize();
	MVector vTangent = sampleFirstDerivative(closestIndex0 / ((m_data.parameterCount - 1) * paramMult));
	double closestProduct = vDisplacement * vTangent;

	if (closestIndex0 == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex0 - 1) / ((m_data.parameterCount - 1) * paramMult));
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex0 == m_data.parameterCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex0 + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex0 + 1) / ((m_data.parameterCount - 1) * paramMult));
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly0 = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate0 = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);

	if (isMinimumEarly0)
	{
		// The minimum occurs before tStable0
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable0 = tStable0 - step * minimumRatio;
	}
	else if (isMinimumLate0)
	{
		// The minimum occurs after tStable0
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable0 = tStable0 + step * minimumRatio;
	}

	// --- Closest Point Joint1 ---
	targetPoint = m_data.controlPoints1[1] + (m_data.controlPoints1[2] - m_data.controlPoints1[1]) / 2;
	// This will always be larger than 0 but may be equal to parameterCount-1
	unsigned int closestIndex1 = (m_data.parameterCount * 3) / 4;
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1];
	closestMag = vDisplacement.length();
	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 - 1];
	earlyMag = vDisplacement.length();

	if (closestIndex1 == m_data.parameterCount - 1)
		lateMag = closestMag;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + 1];
		lateMag = vDisplacement.length();
	}

	direction = earlyMag < closestMag ? -1 : 0;
	direction = lateMag < closestMag ? 1 : direction;

	if (direction != 0)
	{
		double nextMag = direction == -1 ? earlyMag : lateMag;
		while (nextMag < closestMag)
		{
			closestIndex1 += direction;
			closestMag = nextMag;

			// Check bounds
			if (closestIndex1 == 0 && direction == -1)
				break;
			if (closestIndex1 == m_data.parameterCount - 1 && direction == 1)
				break;

			vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + direction];
			nextMag = vDisplacement.length();
		}
	}

	// Same as before, determine the skew
	double tStable1 = closestIndex1 / ((m_data.parameterCount - 1) * paramMult);

	vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1]; vDisplacement.normalize();
	vTangent = sampleFirstDerivative(closestIndex1 / ((m_data.parameterCount - 1) * paramMult));
	closestProduct = vDisplacement * vTangent;

	if (closestIndex1 == 0)
		earlyProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 - 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex1 - 1) / ((m_data.parameterCount - 1) * paramMult));
		earlyProduct = vDisplacement * vTangent;
	}

	if (closestIndex1 == m_data.parameterCount - 1)
		lateProduct = closestProduct;
	else
	{
		vDisplacement = targetPoint - m_data.naturalPoints[closestIndex1 + 1]; vDisplacement.normalize();
		vTangent = sampleFirstDerivative((closestIndex1 + 1) / ((m_data.parameterCount - 1) * paramMult));
		lateProduct = vDisplacement * vTangent;
	}

	bool isMinimumEarly1 = (earlyProduct < 0 && closestProduct > 0) || (earlyProduct > 0 && closestProduct < 0);
	bool isMinimumLate1 = (lateProduct < 0 && closestProduct > 0) || (lateProduct > 0 && closestProduct < 0);

	if (isMinimumEarly1)
	{
		// The minimum occurs before tStable1
		double productDelta = std::abs(earlyProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable1 = tStable1 - step * minimumRatio;
	}
	else if (isMinimumLate1)
	{
		// The minimum occurs after tStable1
		double productDelta = std::abs(lateProduct) + std::abs(closestProduct);
		double minimumRatio = std::abs(closestProduct) / productDelta;
		double step = m_data.parameterRange / (m_data.parameterCount - 1);
		tStable1 = tStable1 + step * minimumRatio;
	}

	// --- Edge Cases ---
	// When both joints overlap each other, the entire second segment of the curve becomes a minimum
	// If this occurs we will ensure all of the samples for the second segment are condensed to this single point
	if (tStable0 > tStable1)
	{
		tStable0 = tStable1;
		closestIndex0 = closestIndex1;
		isMinimumEarly0 = isMinimumEarly1;
		isMinimumLate0 = isMinimumLate1;
	}

	double segment0ArcLength = m_curve.naturalToArcLengthParameter(tStable0 * paramMult, m_data.lengths) * m_data.lengths[m_data.parameterCount - 1];
	double segment1ArcLength = m_curve.naturalToArcLengthParameter(tStable1 * paramMult, m_data.lengths) * m_data.lengths[m_data.parameterCount - 1] - segment0ArcLength;
	double segment2ArcLength = m_data.lengths[m_data.parameterCount - 1] - segment0ArcLength - segment1ArcLength;
	double segmentParamSize = 2.0 / 3.0;

	double targetLength;
	if (splitLengthParameter >= 0.0 && splitLengthParameter < segmentParamSize)
	{
		double segmentRatio = splitLengthParameter / segmentParamSize;
		targetLength = segment0ArcLength * segmentRatio;
	}
	else if (splitLengthParameter >= segmentParamSize && splitLengthParameter < 2 * segmentParamSize)
	{
		double segmentRatio = (splitLengthParameter - segmentParamSize) / segmentParamSize;
		targetLength = segment0ArcLength + segment1ArcLength * segmentRatio;
	}
	else
	{
		double segmentRatio = (splitLengthParameter - 2 * segmentParamSize) / segmentParamSize;
		targetLength = segment0ArcLength + segment1ArcLength + segment2ArcLength * segmentRatio;
	}

	double tLength = 0.0;

	unsigned int low = 0;
	unsigned int high = m_data.parameterCount;
	unsigned int index;

	while (low < high)
	{
		index = low + (high - low) / 2;
		tLength = m_data.lengths[index];

		if (tLength < targetLength)
			low = index + 1;
		else
			high = index;
	}

	if (tLength > targetLength)
		index -= 1;

	assert(m_data.lengths[index] <= targetLength);
	assert(m_data.lengths[index + 1] > targetLength);

	double tLengthBefore = m_data.lengths[index];
	double tLengthAfter = m_data.lengths[index + 1];
	double t = ((double)index + (targetLength - tLengthBefore) / (tLengthAfter - tLengthBefore)) / ((m_data.parameterCount - 1) * 0.5);
	return t;
}

/*	Description
	-----------
	This function is used to calculate a value which represents the stability of the normal up-vector
	This up-vector is used to calculate the principal normal for the RMF calculation
	The output has a range of [0.0, 1.0], where higher values mean the RMF calculation becomes less stable
	This function will return success if orient data is being computed, failure otherwise    */
MStatus FlexiChainDouble::computeNormalStability(double& outStability)
{
	if (!m_data.isOrientEnabled)
		return MStatus::kFailure;

	MVector vLowerBoundTangent = m_data.controlPoints0[1] - m_data.controlPoints0[0];
	vLowerBoundTangent.normalize();
	outStability = m_data.vNormalUp * vLowerBoundTangent;

	return MStatus::kSuccess;
}

/*	Description
	-----------
	This function is used to calculate a value which represents the stability of the counter-twist up-vector
	This up-vector is used to calculate the total twist deviation of the end normal over a time interval
	The output has a range of [0.0, 1.0], where higher values mean the counter-twist calculation becomes less stable
	This function will return success if orient data is being computed, failure otherwise    */
MStatus FlexiChainDouble::computeCounterTwistStability(double& outStability)
{
	if (!m_data.isOrientEnabled)
		return MStatus::kFailure;

	MVector vUpperBoundTangent = m_data.controlPoints1[3] - m_data.controlPoints1[2];
	vUpperBoundTangent.normalize();
	outStability = m_data.vCounterTwistUp * vUpperBoundTangent;

	return MStatus::kSuccess;
}

/*	Description
	-----------
	This function is called exclusively by the FlexiChainDoubleCounterTwist command and is used to calculate the counter twist cache for this node
	Counter twist is an iterative calculation which is designed to stabalise the end frame of the curve
	It essentially mitigates any twist deviation induced on the moving frame of the curve over a specified time range
	This function assumes the command from which it was called has validated all inputs and checked that orientation computation is enabled    */
MStatus FlexiChainDouble::computeCounterTwist(MTime startTime, MTime endTime, MTime timeStep, MObject& animCurveObj, MAnimCurveChange& animMod)
{
	MStatus status;

	MFnAnimCurve fnAnimCurve{ animCurveObj };
	MTime currentTime = MAnimControl::currentTime();
	MTime iterTime = startTime;
	double previousAngle = 0.0;
	double totalAngle = 0.0;

	// For each frame, calculate a signed angle between the upVector/endUpVectorOverride (projected onto the tangent plane) and the normal vector
	// Note, if the up-vector is close to parallel with tangent, the projection onto the tangent plane will be close to a zero-vector (angle may experience flipping)
	while (iterTime <= endTime)
	{
		if (!status)
			return MStatus::kFailure;

		MDataBlock dataBlock = forceCache();

		// This call does not cause the viewport loop to refresh
		MAnimControl::setCurrentTime(iterTime);
		// We must force a DG update (we can either call the "refresh" command or manually pull data from an output)
		dataBlock.inputValue(outputWorldFramesAttr);

		// Calculate upper threshold data
		MVector vUpperBoundTangent = m_data.controlPoints1[3] - m_data.controlPoints1[2];
		vUpperBoundTangent.normalize();
		MQuaternion qPrincipalNormal{ m_data.vPrincipalNormal.x, m_data.vPrincipalNormal.y, m_data.vPrincipalNormal.z, 0.0 };
		MQuaternion qReflectionComposition = m_data.rmfReflections[m_data.sampleCount - 1];
		MQuaternion qReflectionCompositionConjugate = qReflectionComposition.conjugate();
		MQuaternion qNormalProjected = MRS::quaternionMultiply(qReflectionComposition, qPrincipalNormal);
		MQuaternion qNormal = MRS::quaternionMultiply(qNormalProjected, qReflectionCompositionConjugate);
		MVector vUpperBoundNormal{ qNormal.x, qNormal.y, qNormal.z };
		vUpperBoundNormal.normalize();

		// Project the up-vector onto the end tangent
		MVector vProjUpOnTangent = (m_data.vCounterTwistUp * vUpperBoundTangent) * vUpperBoundTangent;
		// Project the up-vector onto the tangent plane
		MVector vProjUpOnTangentPlane = m_data.vCounterTwistUp - vProjUpOnTangent;
		vProjUpOnTangentPlane.normalize();

		// Calculate sign of the angle
		MVector vCrossProjWithNormal = vProjUpOnTangentPlane ^ vUpperBoundNormal;
		double dotCrossWithTangent = vCrossProjWithNormal * vUpperBoundTangent;
		int angularDirection = dotCrossWithTangent < 0.0 ? 1 : -1;

		// Calculate angle
		double currentAngle = std::acos(vProjUpOnTangentPlane * vUpperBoundNormal);
		currentAngle *= angularDirection;

		// Calculate delta from previous frame
		double angularDelta;
		if (std::abs(currentAngle - previousAngle) > M_PI)
		{
			// The normal is pointing in a similiar direction to the negated projection vector
			// This causes a large discrepency between the previous and current angles (ie. angle is close to 180 degrees but direction has changed)
			if (currentAngle > 0.0)
				angularDelta = (-2 * M_PI) + (currentAngle - previousAngle);
			else
				angularDelta = (2 * M_PI) + (currentAngle - previousAngle);
		}
		else
			angularDelta = currentAngle - previousAngle;

		// Counter the twist
		angularDelta *= -1;

		// Set a keyframe at the current time using the accumulative angle
		totalAngle += angularDelta;
		fnAnimCurve.addKey(iterTime, totalAngle, MFnAnimCurve::kTangentLinear, MFnAnimCurve::kTangentLinear, &animMod, &status);

		previousAngle = currentAngle;
		iterTime += timeStep;
	}

	// Restore the previous time
	MAnimControl::setCurrentTime(currentTime);

	return status;
}

/*	Description
	-----------
	Samples a point on the internal curve using previously evaluated data

	Args
	----
	t = Natural parameter of curve, range = [0,2]    */
MVector FlexiChainDouble::sampleCurve(double t) const
{
	MVector vCurveSample;
	if (t <= 1.0)
		vCurveSample = m_curve.sampleCurve(t, m_data.jointVolume0, m_data.jointVolume0, m_data.controlPoints0);
	else
		vCurveSample = m_curve.sampleCurve(t - 1.0, m_data.jointVolume1, m_data.jointVolume1, m_data.controlPoints1);

	return vCurveSample;
}

/*	Description
	-----------
	Samples the first derivative of the internal curve using previously evaluated data
	The returned vector will not be normalized

	Args
	----
	t = Natural parameter of curve, range = [0,2]    */
MVector FlexiChainDouble::sampleFirstDerivative(double t) const
{
	MVector vTangent;

	if (t <= 1.0)
		vTangent = m_curve.sampleFirstDerivative(t, m_data.jointVolume0, m_data.jointVolume0, m_data.controlPoints0);
	else
		vTangent = m_curve.sampleFirstDerivative(t - 1.0, m_data.jointVolume1, m_data.jointVolume1, m_data.controlPoints1);

	return vTangent;
}

void FlexiChainDouble::instancingChangedCallback(MDagPath& child, MDagPath& parent, void* clientData)
{
	MGlobal::displayWarning("FlexiChainDoubleShape does not support instancing!");
}

const MRS::CubicTBezier& FlexiChainDouble::getCurve() const { return m_curve; }
const FlexiChainDouble::FlexiChainDouble_Data& FlexiChainDouble::getCurveData() const { return m_data; }
MDataBlock FlexiChainDouble::getDataBlock() { return forceCache(); }

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The FlexiChainDoubleCounterTwist command is used to calculate the counter twist cache for a given FlexiChainDouble node
	Counter twist is an iterative calculation which is designed to stabalise the end frame of the curve
	It essentially mitigates any twist deviation induced on the moving frame of the curve over a specified time range

	MEL Command
	-----------
	FlexiChainDoubleCounterTwist [-startTime float] [-endTime float] [-timeStep float] [object]

	Flags
	-----
	-startTime(-st)
		This flag specifies the time at which the first keyframe will be set for the cache
		This flag is required

	-endTime(-et)
		This flag specifies the last possible time at which a keyframe may be set for the cache
		If the delta between the startTime and endTime is not evenly divisible by the timeStep, the last keyframe will occur before the endTime
		This flag is required

	-timeStep(-st)
		This flag specifies the interval at which keyframes will be set between the startTime (inclusive) and endTime (inclusive)
		A precautionary minimum value of 0.01 is enforced so that extremely small values do not tend towards an infinitely sized cached
		The default value is 1.0

	Args
	----
	object
		This argument should be used to specify the name of the node for which this command is to calculate and set counter-twist data
*/

FlexiChainDouble_CounterTwistCommand::FlexiChainDouble_CounterTwistCommand() : MPxCommand() {}
FlexiChainDouble_CounterTwistCommand::~FlexiChainDouble_CounterTwistCommand() {}

// ------ Registration ------

const char* FlexiChainDouble_CounterTwistCommand::kStartTimeFlag = "-st";
const char* FlexiChainDouble_CounterTwistCommand::kStartTimeFlagLong = "-startTime";
const char* FlexiChainDouble_CounterTwistCommand::kEndTimeFlag = "-et";
const char* FlexiChainDouble_CounterTwistCommand::kEndTimeFlagLong = "-endTime";
const char* FlexiChainDouble_CounterTwistCommand::kTimeStepFlag = "-ts";
const char* FlexiChainDouble_CounterTwistCommand::kTimeStepFlagLong = "-timeStep";

MSyntax FlexiChainDouble_CounterTwistCommand::newSyntax()
{
	MSyntax syntax;

	// Flags
	syntax.addFlag(kStartTimeFlag, kStartTimeFlagLong, MSyntax::kTime);
	syntax.addFlag(kEndTimeFlag, kEndTimeFlagLong, MSyntax::kTime);
	syntax.addFlag(kTimeStepFlag, kTimeStepFlagLong, MSyntax::kTime);

	// Args
	syntax.useSelectionAsDefault(true);
	syntax.setObjectType(MSyntax::kSelectionList, 1, 1);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

// ------ MPxCommand ------

#define kErrorFlagNotSet \
	"The \"-startTime\" and \"-endTime\" flags are required by this command."

#define kErrorParsingFlag \
	"Error parsing flag \"^1s\"."

#define kErrorInvalidTimeStep \
	"The \"-timeStep\" flag must be given a value greater or equal to 0.01 ."

#define kErrorInvalidTimeInterval \
	"The \"-endTime\" flag must be given a value greater than the \"-startTime\" flag"

#define kErrorNoValidObject \
	"This command requires a single FlexiChainDouble or its transform node to be specified or selected."

#define kErrorInvalidType \
	"Object \"^1s\" has an invalid type. Only a FlexiChainDouble or its transform can be specified."

#define kErrorInvalidConnection \
	"Plug \"^1s.counterTwist\" has an incompatible destination connection from \"^2s\". Only animCurve connections will be overridden."

#define kErrorOrientationDisabled \
	"Plug \"^1s.computeOrientation\" holds false value. Ensure orientation is being computed by this node before proceeding."

bool FlexiChainDouble_CounterTwistCommand::isUndoable() const
{
	return true;
}

MStatus FlexiChainDouble_CounterTwistCommand::doIt(const MArgList& args)
{
	MStatus status;

	// Argument list parser
	MArgDatabase argParser(syntax(), args);

	// Check if required flags are set
	if (!argParser.isFlagSet(kStartTimeFlagLong) || !argParser.isFlagSet(kEndTimeFlagLong))
	{
		displayError(kErrorFlagNotSet);
		return MStatus::kFailure;
	}

	// Parse command flags
	MTime startTime;
	startTime.setUnit(MTime::uiUnit());
	if (!argParser.getFlagArgument(kStartTimeFlagLong, 0, startTime))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kStartTimeFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	MTime endTime;
	endTime.setUnit(MTime::uiUnit());
	if (!argParser.getFlagArgument(kEndTimeFlagLong, 0, endTime))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kEndTimeFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}
	MTime timeStep{ 1.0, MTime::uiUnit() };
	if (argParser.isFlagSet(kTimeStepFlagLong))
	{
		if (!argParser.getFlagArgument(kTimeStepFlagLong, 0, timeStep))
		{
			MString msg;
			MString msgFormat = kErrorParsingFlag;
			msg.format(msgFormat, kTimeStepFlagLong);
			displayError(msg);
			return MStatus::kFailure;
		}
	}

	// Check parsed values are valid
	if (timeStep < 0.01)
	{
		displayError(kErrorInvalidTimeStep);
		return MStatus::kFailure;
	}

	if (endTime < startTime)
	{
		displayError(kErrorInvalidTimeInterval);
		return MStatus::kFailure;
	}

	// Parse specified object from either command arg or current selection
	MSelectionList selectionList;
	argParser.getObjects(selectionList);

	// Check if a node is selected
	if (selectionList.length() == 0)
	{
		displayError(kErrorNoValidObject);
		return MStatus::kFailure;
	}

	// Check if a plugin shape or a transform of a plugin shape is selected
	MDagPath path;
	selectionList.getDagPath(0, path);
	MSelectionList firstNodeSelectionList;
	firstNodeSelectionList.add(path);
	// The node type filter will cause the shape to be iterated if the transform of a custom plugin shape is selected
	MItSelectionList iter{ firstNodeSelectionList, MFn::kPluginDependNode };
	if (iter.isDone())
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	// Check if the selected plugin shape is a FlexiChainDouble node
	MDagPath pluginPath;
	iter.getDagPath(pluginPath);
	MFnDependencyNode fnDep{ pluginPath.node() };
	if (fnDep.typeId() != FlexiChainDouble::kTypeId)
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	FlexiChainDouble* locator = (FlexiChainDouble*)fnDep.userNode();

	// Check orient is enabled for the node
	// Note, orient computation is not a keyable attribute as its state must remain true during the counter-twist calculation
	MPlug orientEnabledPlug{ pluginPath.node(), FlexiChainDouble::computeOrientationAttr };
	if (!orientEnabledPlug.asBool())
	{
		MString msg;
		MString msgFormat = kErrorOrientationDisabled;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	// If an anim curve is connected, disconnect and delete it, otherwise error if the connection is not an anim curve
	MPlug counterTwistPlug{ pluginPath.node(), FlexiChainDouble::counterTwistAttr };
	if (counterTwistPlug.isDestination())
	{
		MPlug sourcePlug = counterTwistPlug.sourceWithConversion();
		MObject sourceNodeObj = sourcePlug.node();
		if (sourceNodeObj.hasFn(MFn::kAnimCurveTimeToAngular))
		{
			// Note, disconnect and delete requires an intermediary doIt() invocation (otherwise the connected node may also be deleted)
			status = m_animDGMod.disconnect(sourcePlug, counterTwistPlug);
			if (status)
				status = m_animDGMod.doIt();
			if (status)
				status = m_animDGMod.deleteNode(sourceNodeObj);
		}
		else
		{
			MString msg;
			MString msgFormat = kErrorInvalidConnection;
			msg.format(msgFormat, pluginPath.partialPathName(), sourcePlug.partialName(true, false, false, false, false, true));
			displayError(msg);
			return MStatus::kFailure;
		}
	}

	// Create a new animCurve
	MObject animCurveObj;
	if (status)
	{
		// MFnAnimCurve::create() will invoke doIt() on the modifier, executing the above delete operation, creating a new anim curve and connecting it to the plug
		MFnAnimCurve fnAnimCurve;
		animCurveObj = fnAnimCurve.create(counterTwistPlug, MFnAnimCurve::kAnimCurveTA, &m_animDGMod, &status);
	}

	// Add keyframes and store the changes in a cache for undo/redo
	if (status)
		status = locator->computeCounterTwist(startTime, endTime, timeStep, animCurveObj, m_animChangeMod);

	return status;
}

MStatus FlexiChainDouble_CounterTwistCommand::redoIt()
{
	MStatus status;

	status = m_animDGMod.doIt();
	if (status)
	{
		status = m_animChangeMod.redoIt();
	}

	return status;
}

MStatus FlexiChainDouble_CounterTwistCommand::undoIt()
{
	MStatus status;

	status = m_animChangeMod.undoIt();
	if (status)
	{
		status = m_animDGMod.undoIt();
	}

	return status;
}

// Cleanup
#undef kErrorFlagNotSet
#undef kErrorParsingFlag
#undef kErrorInvalidTimeStep
#undef kErrorInvalidTimeInterval
#undef kErrorNoValidObject
#undef kErrorInvalidType
#undef kErrorInvalidConnection
#undef kErrorOrientationDisabled

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The FlexiChainDoubleStability command is used to query the stability of the computed orient data
	It is designed for debugging purposes and is not undoable (ie. query only)
	Stability is effected by two calculations:
	- The up-vector used to calculate the principal normal for the RMF calculation
	- The counter-twist target vector which is used to iteratively calculate the total twist deviation of the end normal over a time interval

	MEL Command
	-----------
	FlexiChainDoubleStability [-time float] [-normal] [-counterTwist] [object]

	Flags
	-----
	-time(-t)
		This flag specifies the time at which to query the stability of the up-vector
		This flag is required

	-normal(-n)
		This flag is used to query the normal up-vector stability of the given node

	-counterTwist(-ct)
		This flag is used to query the counter-twist up-vector stability of the given node

	Args
	----
	object
		This argument should be used to specify the name of the node for which this command is to query
*/

FlexiChainDouble_StabilityCommand::FlexiChainDouble_StabilityCommand() {}
FlexiChainDouble_StabilityCommand::~FlexiChainDouble_StabilityCommand() {}

// ------ Registration ------

const char* FlexiChainDouble_StabilityCommand::kTimeFlag = "-t";
const char* FlexiChainDouble_StabilityCommand::kTimeFlagLong = "-time";
const char* FlexiChainDouble_StabilityCommand::kNormalFlag = "-n";
const char* FlexiChainDouble_StabilityCommand::kNormalFlagLong = "-normal";
const char* FlexiChainDouble_StabilityCommand::kCounterTwistFlag = "-ct";
const char* FlexiChainDouble_StabilityCommand::kCounterTwistFlagLong = "-counterTwist";

MSyntax FlexiChainDouble_StabilityCommand::newSyntax()
{
	MSyntax syntax;

	// Flags
	syntax.addFlag(kTimeFlag, kTimeFlagLong, MSyntax::kTime);
	syntax.addFlag(kNormalFlag, kNormalFlagLong, MSyntax::kNoArg);
	syntax.addFlag(kCounterTwistFlag, kCounterTwistFlagLong, MSyntax::kNoArg);

	// Args
	syntax.useSelectionAsDefault(true);
	syntax.setObjectType(MSyntax::kSelectionList, 1, 1);

	syntax.enableQuery(true);
	syntax.enableEdit(false);

	return syntax;
}

// ------ MPxCommand ------

#define kErrorInvalidFlagCombination \
	"Only one of -normal/-counterTwist flags may be specified."

#define kErrorFlagNotSet \
	"The \"-time\" flag is required by this command."

#define kErrorQueryOnly \
	"This command is query only, must use the \"-query\" flag."

#define kErrorParsingFlag \
	"Error parsing flag \"^1s\"."

#define kErrorNoValidObject \
	"This command requires a single FlexiChainDouble or its transform node to be specified or selected."

#define kErrorInvalidType \
	"Object \"^1s\" has an invalid type. Only a FlexiChainDouble or its transform can be specified."

#define kErrorOrientationDisabled \
	"Plug \"^1s.computeOrientation\" holds false value. Ensure orientation is being computed by this node before proceeding."

MStatus FlexiChainDouble_StabilityCommand::doIt(const MArgList& args)
{
	MStatus status;

	// Argument list parser
	MArgDatabase argParser(syntax(), args);

	// Validate flags
	if (argParser.isFlagSet(kNormalFlagLong) && argParser.isFlagSet(kCounterTwistFlagLong))
	{
		MGlobal::displayError(kErrorInvalidFlagCombination);
		return MStatus::kFailure;
	}

	if (!argParser.isFlagSet(kTimeFlagLong))
	{
		displayError(kErrorFlagNotSet);
		return MStatus::kFailure;
	}

	if (!argParser.isQuery())
	{
		displayError(kErrorQueryOnly);
		return MStatus::kFailure;
	}

	// Parse command flags
	bool isQueryNormal = argParser.isFlagSet(kNormalFlagLong);

	MTime time;
	time.setUnit(MTime::uiUnit());
	if (!argParser.getFlagArgument(kTimeFlagLong, 0, time))
	{
		MString msg;
		MString msgFormat = kErrorParsingFlag;
		msg.format(msgFormat, kTimeFlagLong);
		displayError(msg);
		return MStatus::kFailure;
	}

	// Parse specified object from either command arg or current selection
	MSelectionList selectionList;
	argParser.getObjects(selectionList);

	// Check if a node is selected
	if (selectionList.length() == 0)
	{
		displayError(kErrorNoValidObject);
		return MStatus::kFailure;
	}

	// Check if a plugin shape or a transform of a plugin shape is selected
	MDagPath path;
	selectionList.getDagPath(0, path);
	MSelectionList firstNodeSelectionList;
	firstNodeSelectionList.add(path);
	// The node type filter will cause the shape to be iterated if the transform of a custom plugin shape is selected
	MItSelectionList iter{ firstNodeSelectionList, MFn::kPluginDependNode };
	if (iter.isDone())
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	// Check if the selected plugin shape is a FlexiChainDouble node
	MDagPath pluginPath;
	iter.getDagPath(pluginPath);
	MFnDependencyNode fnDep{ pluginPath.node() };
	if (fnDep.typeId() != FlexiChainDouble::kTypeId)
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	FlexiChainDouble* locator = (FlexiChainDouble*)fnDep.userNode();
	MDataBlock dataBlock = locator->_forceCache();

	// Check orient is enabled for the node
	bool isOrientEnabled = dataBlock.inputValue(FlexiChainDouble::computeOrientationAttr).asBool();
	if (!isOrientEnabled)
	{
		MString msg;
		MString msgFormat = kErrorOrientationDisabled;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	MTime currentTime = MAnimControl::currentTime();
	// This call does not cause the viewport loop to refresh
	MAnimControl::setCurrentTime(time);
	// We must force a DG update (we can either call the "refresh" command or manually pull data from an output)
	dataBlock.inputValue(FlexiChainDouble::outputWorldFramesAttr);

	// Calculate the stability of the up-vector
	double stability;
	if (isQueryNormal)
		locator->computeNormalStability(stability);
	else
		locator->computeCounterTwistStability(stability);

	// Set result and cleanup
	setResult(stability);
	MAnimControl::setCurrentTime(currentTime);
	return status;
}

// Cleanup
#undef kErrorInvalidFlagCombination
#undef kErrorFlagNotSet
#undef kErrorQueryOnly
#undef kErrorParsingFlag
#undef kErrorNoValidObject
#undef kErrorInvalidType
#undef kErrorOrientationDisabled

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------