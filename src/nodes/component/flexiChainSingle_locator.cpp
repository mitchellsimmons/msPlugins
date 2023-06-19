#include "flexiChainSingle_locator.h"

/*	Description
	-----------
	This node approximates and draws a cubic trigonemtric Bezier with two shaping parameters
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
	controlPoint0, controlPoint1, controlPoint2 - vector
		Specifies the three local space positions which determine the hull of the T-Bezier and help define its shape

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
		Split Arc Length	(2)		The curve will be split into two segments at the joint
									Each segment will be parameterized in terms of the segment length using uniform distance

	rotateOrder - enum
		Specifies the composition order of rotations used to produce each of the euler values output by the orientation attributes

	jointOffset - double
		Specifies a multiplier which is used to offset each of the virtual points away from the physical point
		- The offset direction is defined by the joint angle, such that rotating one edge half way towards the other defines the direction
		- The offset distance is proportional to both the given value and the joint angle
		- An angle of pi will act as a zero multiplier to the offset, whilst an angle of zero will produce a maximum offset

	jointRadius - distance
		Specifies the distance of each virtual point away from the physical point
		Used to affect the curvature of the joint, whereby larger values result in a greater curvature
		- The offset direction is perpendicular to the jointOffset direction

	jointVolume -double [-1.0, 1.0]
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

FlexiChainSingle::FlexiChainSingle() :
	m_instanceAddedCallbackId{ 0 }
{}

FlexiChainSingle::~FlexiChainSingle()
{
	if (m_instanceAddedCallbackId != 0)
	{
		MMessage::removeCallback(m_instanceAddedCallbackId);
		m_instanceAddedCallbackId = 0;
	}
}FlexiChainSingle::FlexiChainSingle_Data::FlexiChainSingle_Data() :
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
	jointVolume{ 0.0 },
	controlPoints{ 4 },
	currentParameters{ nullptr }
{}

FlexiChainSingle::FlexiChainSingle_Data::~FlexiChainSingle_Data() {}

// ------ Constants ------
const unsigned int FlexiChainSingle::FlexiChainSingle_Data::order = 3;
const unsigned int FlexiChainSingle::FlexiChainSingle_Data::degree = 3;

// ------ Attr ------
MObject FlexiChainSingle::controlPoint0Attr;
MObject FlexiChainSingle::controlPoint0XAttr;
MObject FlexiChainSingle::controlPoint0YAttr;
MObject FlexiChainSingle::controlPoint0ZAttr;
MObject FlexiChainSingle::controlPoint1Attr;
MObject FlexiChainSingle::controlPoint1XAttr;
MObject FlexiChainSingle::controlPoint1YAttr;
MObject FlexiChainSingle::controlPoint1ZAttr;
MObject FlexiChainSingle::controlPoint2Attr;
MObject FlexiChainSingle::controlPoint2XAttr;
MObject FlexiChainSingle::controlPoint2YAttr;
MObject FlexiChainSingle::controlPoint2ZAttr;
MObject FlexiChainSingle::jointOffsetAttr;
MObject FlexiChainSingle::jointRadiusAttr;
MObject FlexiChainSingle::jointVolumeAttr;
MObject FlexiChainSingle::upVectorAttr;
MObject FlexiChainSingle::upVectorXAttr;
MObject FlexiChainSingle::upVectorYAttr;
MObject FlexiChainSingle::upVectorZAttr;
MObject FlexiChainSingle::rotateOrderAttr;
MObject FlexiChainSingle::computeOrientationAttr;
MObject FlexiChainSingle::subdivisionsAttr;
MObject FlexiChainSingle::outputCountAttr;
MObject FlexiChainSingle::parameterizationAttr;
MObject FlexiChainSingle::normalUpVectorOverrideCompoundAttr;
MObject FlexiChainSingle::normalUpVectorOverrideStateAttr;
MObject FlexiChainSingle::normalUpVectorOverrideAttr;
MObject FlexiChainSingle::normalUpVectorOverrideXAttr;
MObject FlexiChainSingle::normalUpVectorOverrideYAttr;
MObject FlexiChainSingle::normalUpVectorOverrideZAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideCompoundAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideStateAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideXAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideYAttr;
MObject FlexiChainSingle::counterTwistUpVectorOverrideZAttr;
MObject FlexiChainSingle::counterTwistBlendAttr;
MObject FlexiChainSingle::counterTwistAttr;
MObject FlexiChainSingle::startTwistAttr;
MObject FlexiChainSingle::endTwistAttr;
MObject FlexiChainSingle::rollAttr;
MObject FlexiChainSingle::drawRibbonAttr;
MObject FlexiChainSingle::drawCurveAttr;
MObject FlexiChainSingle::drawNormalsAttr;
MObject FlexiChainSingle::drawHullAttr;
MObject FlexiChainSingle::drawSpaceTransformationAttr;
MObject FlexiChainSingle::customDrawSpaceTransformAttr;
MObject FlexiChainSingle::computeTwistAdjustmentsAttr;
MObject FlexiChainSingle::twistAdjustmentCompoundAttr;
MObject FlexiChainSingle::twistAdjustmentRampAttr;
MObject FlexiChainSingle::twistAdjustmentRampPositionAttr;
MObject FlexiChainSingle::twistAdjustmentRampValueAttr;
MObject FlexiChainSingle::twistAdjustmentRampInterpolationAttr;
MObject FlexiChainSingle::twistAdjustmentValueAttr;
MObject FlexiChainSingle::twistAdjustmentOffsetAttr;
MObject FlexiChainSingle::twistAdjustmentFalloffModeAttr;
MObject FlexiChainSingle::twistAdjustmentFalloffDistanceAttr;
MObject FlexiChainSingle::twistAdjustmentRepeatAttr;
MObject FlexiChainSingle::computeScaleAdjustmentsAttr;
MObject FlexiChainSingle::scaleAdjustmentCompoundAttr;
MObject FlexiChainSingle::scaleAdjustmentRampAttr;
MObject FlexiChainSingle::scaleAdjustmentRampPositionAttr;
MObject FlexiChainSingle::scaleAdjustmentRampValueAttr;
MObject FlexiChainSingle::scaleAdjustmentRampInterpolationAttr;
MObject FlexiChainSingle::scaleAdjustmentValueAttr;
MObject FlexiChainSingle::scaleAdjustmentValueXAttr;
MObject FlexiChainSingle::scaleAdjustmentValueYAttr;
MObject FlexiChainSingle::scaleAdjustmentValueZAttr;
MObject FlexiChainSingle::scaleAdjustmentOffsetAttr;
MObject FlexiChainSingle::scaleAdjustmentFalloffModeAttr;
MObject FlexiChainSingle::scaleAdjustmentFalloffDistanceAttr;
MObject FlexiChainSingle::scaleAdjustmentRepeatAttr;
MObject FlexiChainSingle::evalSinceDirtyAttr;
MObject FlexiChainSingle::drawSinceEvalAttr;
MObject FlexiChainSingle::outputLocalFramesAttr;
MObject FlexiChainSingle::outputLocalPositionsAttr;
MObject FlexiChainSingle::outputLocalOrientationsAttr;
MObject FlexiChainSingle::outputLocalScalesAttr;
MObject FlexiChainSingle::outputWorldFramesAttr;
MObject FlexiChainSingle::outputWorldPositionsAttr;
MObject FlexiChainSingle::outputWorldOrientationsAttr;
MObject FlexiChainSingle::outputWorldScalesAttr;

// ------ MPxNode ------
MPxNode::SchedulingType FlexiChainSingle::schedulingType() const
{
	return SchedulingType::kParallel;
}

void FlexiChainSingle::postConstructor()
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

MStatus FlexiChainSingle::initialize()
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
	createDoubleAttribute(jointOffsetAttr, "jointOffset", "jointOffset", 0.0, kDefaultPreset | kKeyable);
	createDistanceAttribute(jointRadiusAttr, "jointRadius", "jointRadius", 10.0, kDefaultPreset | kKeyable);
	createDoubleAttribute(jointVolumeAttr, "jointVolume", "jointVolume", 0.0, kDefaultPreset | kKeyable);
	setMinMax<double>(jointVolumeAttr, -1.0, 1.0);

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
	addAttribute(upVectorAttr);
	addAttribute(computeOrientationAttr);
	addAttribute(rotateOrderAttr);
	addAttribute(outputCountAttr);
	addAttribute(subdivisionsAttr);
	addAttribute(parameterizationAttr);
	addAttribute(jointOffsetAttr);
	addAttribute(jointRadiusAttr);
	addAttribute(jointVolumeAttr);
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
	attributeAffects(jointOffsetAttr, outputLocalFramesAttr);
	attributeAffects(jointRadiusAttr, outputLocalFramesAttr);
	attributeAffects(jointVolumeAttr, outputLocalFramesAttr);
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
	attributeAffects(jointOffsetAttr, outputLocalPositionsAttr);
	attributeAffects(jointRadiusAttr, outputLocalPositionsAttr);
	attributeAffects(jointVolumeAttr, outputLocalPositionsAttr);
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
	attributeAffects(jointOffsetAttr, outputLocalOrientationsAttr);
	attributeAffects(jointRadiusAttr, outputLocalOrientationsAttr);
	attributeAffects(jointVolumeAttr, outputLocalOrientationsAttr);
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
	attributeAffects(jointOffsetAttr, outputLocalScalesAttr);
	attributeAffects(jointRadiusAttr, outputLocalScalesAttr);
	attributeAffects(jointVolumeAttr, outputLocalScalesAttr);
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
	attributeAffects(jointOffsetAttr, outputWorldFramesAttr);
	attributeAffects(jointRadiusAttr, outputWorldFramesAttr);
	attributeAffects(jointVolumeAttr, outputWorldFramesAttr);
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
	attributeAffects(jointOffsetAttr, outputWorldPositionsAttr);
	attributeAffects(jointRadiusAttr, outputWorldPositionsAttr);
	attributeAffects(jointVolumeAttr, outputWorldPositionsAttr);
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
	attributeAffects(jointOffsetAttr, outputWorldOrientationsAttr);
	attributeAffects(jointRadiusAttr, outputWorldOrientationsAttr);
	attributeAffects(jointVolumeAttr, outputWorldOrientationsAttr);
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
	attributeAffects(jointOffsetAttr, outputWorldScalesAttr);
	attributeAffects(jointRadiusAttr, outputWorldScalesAttr);
	attributeAffects(jointVolumeAttr, outputWorldScalesAttr);
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
MStatus FlexiChainSingle::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	// Highest priority attributes first
	if (plug == controlPoint0Attr || plug == controlPoint0XAttr || plug == controlPoint0YAttr || plug == controlPoint0ZAttr ||
		plug == controlPoint1Attr || plug == controlPoint1XAttr || plug == controlPoint1YAttr || plug == controlPoint1ZAttr ||
		plug == controlPoint2Attr || plug == controlPoint2XAttr || plug == controlPoint2YAttr || plug == controlPoint2ZAttr ||
		plug == upVectorAttr || plug == upVectorXAttr || plug == upVectorYAttr || plug == upVectorZAttr ||
		plug == startTwistAttr ||
		plug == endTwistAttr ||
		plug == counterTwistAttr ||
		plug == rollAttr ||
		plug == jointOffsetAttr ||
		plug == jointRadiusAttr ||
		plug == jointVolumeAttr ||
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
MStatus FlexiChainSingle::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
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
			(evaluationNode.dirtyPlugExists(upVectorAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorXAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorYAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(upVectorZAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(startTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(endTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(counterTwistAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(rollAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointOffsetAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointRadiusAttr, &status) && status) ||
			(evaluationNode.dirtyPlugExists(jointVolumeAttr, &status) && status) ||
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

MStatus FlexiChainSingle::compute(const MPlug& plug, MDataBlock& dataBlock)
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
				outData[j] = MPoint{ m_data.frames[j][3] } * worldTransform;

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

bool FlexiChainSingle::isBounded() const
{
	return true;
}

/*	Description
	-----------
	Beziers are contained within a convex hull defined by all of their control points    */
MBoundingBox FlexiChainSingle::boundingBox() const
{
	MBoundingBox bbox;

	for (unsigned int i = 0; i < m_data.controlPoints.size(); i++)
		bbox.expand(m_data.controlPoints[i]);

	return bbox;
}

// ------ Helpers ------

/*	Description
	------------
	This function is responsible for computing curve data based on the current input values
	The method is seperate from compute as our draw override needs to be able request updated data without cleaning the output attributes
	Basic state tracking has been implemented so that the draw cycle can query whether the function has already been invoked by MPxNode::compute()    */
void FlexiChainSingle::computeCurveData(MDataBlock& dataBlock)
{
	// --- Counts ---
	m_data.subdivisions = (unsigned)dataBlock.inputValue(subdivisionsAttr).asInt();
	m_data.outputCount = (unsigned)dataBlock.inputValue(outputCountAttr).asInt();
	m_data.parameterCount = m_data.outputCount + (m_data.outputCount - 1) * m_data.subdivisions;
	assert(m_data.parameterCount >= 2);

	// --- Shaping Parameters ---
	m_data.jointVolume = dataBlock.inputValue(jointVolumeAttr).asDouble();

	// --- Physical Points ---
	MVector P0 = dataBlock.inputValue(controlPoint0Attr).asVector();
	MVector P1 = dataBlock.inputValue(controlPoint1Attr).asVector();
	MVector P2 = dataBlock.inputValue(controlPoint2Attr).asVector();
	
	MVector vP1ToP0 = P0 - P1;
	vP1ToP0.normalize();
	MVector vP1ToP2 = P2 - P1;
	vP1ToP2.normalize();
	MVector vP0ToP2 = P2 - P0;
	vP0ToP2.normalize();

	// Determine the four virtual points from the three physical points
	// Rotate the P1ToP0 vector by the offsetAngle (using Rodrigues formula) to find the offset direction of the second control point
	// We rotate the vector in both directions so that we can maximize the dot product delta of both results with the P1ToP2 vector
	double jointAngle = acos(vP1ToP0 * vP1ToP2);
	double offsetAngle = M_PI_2 - jointAngle / 2;
	double dotJoint = vP1ToP0 * vP1ToP2;
	double cosOffsetAngle = cos(-offsetAngle);
	double sinOffsetAngle = sin(-offsetAngle);
	double cosOffsetCounterAngle = cos(offsetAngle);
	double sinOffsetCounterAngle = sin(offsetAngle);
	MVector vRotPlaneNormal = vP1ToP0 ^ vP1ToP2;
	vRotPlaneNormal.normalize();
	MVector vCrossNormalWithP1ToP0 = vRotPlaneNormal ^ vP1ToP0;
	double dotNormalWithP1ToP0 = vRotPlaneNormal * vP1ToP0;
	MVector vRotatedP1ToP0 = vP1ToP0 * cosOffsetAngle + vCrossNormalWithP1ToP0 * sinOffsetAngle +
		vRotPlaneNormal * dotNormalWithP1ToP0 * (1.0 - cosOffsetAngle);
	MVector vCounterRotatedP1ToP0 = vP1ToP0 * cosOffsetCounterAngle + vCrossNormalWithP1ToP0 * sinOffsetCounterAngle +
		vRotPlaneNormal * dotNormalWithP1ToP0 * (1.0 - cosOffsetCounterAngle);

	// Find the maximum dot product delta and determine the offset directions
	MVector vLateralOffsetDirection;
	MVector vPerpendicularOffsetDirection;
	if (dotJoint - vRotatedP1ToP0 * vP1ToP2 < dotJoint - vCounterRotatedP1ToP0 * vP1ToP2)
	{
		vLateralOffsetDirection = vCounterRotatedP1ToP0;
		vPerpendicularOffsetDirection = vRotPlaneNormal ^ vRotatedP1ToP0;
	}
	else
	{
		vLateralOffsetDirection = vRotatedP1ToP0;
		vPerpendicularOffsetDirection = vRotatedP1ToP0 ^ vRotPlaneNormal;
	}

	// Offset the mid point along the perpendicular vector
	// When the jointOffset attribute is set to a value of 1.0 the offset will roughly align the curve so that it passes through the mid point
	// Note, this is a very approximate calculation and is only true when the jointVolume attribute is also set to 1.0
	// To determine an exact value, we would need to sample the curve twice so that we could find the closest point on the curve without the offset applied
	double jointOffsetMultiplier = dataBlock.inputValue(jointOffsetAttr).asDouble();
	double theta = acos(vP1ToP0 * -vP0ToP2);
	double alpha = M_PI - (jointAngle / 2) - theta;
	// We are finding the vector which goes from the mid point and intersects the startToEnd vector, then dividing by a constant to meet the above condition
	double jointOffset = ((P0 - P1).length() / sin(alpha)) * sin(theta) * jointOffsetMultiplier / 19.0;
	P1 = P1 + jointOffset * vPerpendicularOffsetDirection;

	// --- Virtual Points ---
	double jointRadius = dataBlock.inputValue(jointRadiusAttr).asDouble();

	m_data.controlPoints[0] = P0;
	m_data.controlPoints[1] = P1 + vLateralOffsetDirection * jointRadius;
	m_data.controlPoints[2] = P1 + vLateralOffsetDirection * jointRadius * -1;
	m_data.controlPoints[3] = P2;

	//		Parameterization
	m_data.parameterization = dataBlock.inputValue(parameterizationAttr).asShort();

	// Could be optimized for when arc-length parameterization is disabled however this is rare, therefore simplify for the general case
	m_data.naturalPoints.resize(m_data.parameterCount);
	m_data.lengths.resize(m_data.parameterCount);
	m_data.naturalParameters.resize(m_data.parameterCount);
	m_data.arcLengthParameters.resize(m_data.parameterCount);
	m_data.splitLengthParameters.resize(m_data.parameterCount);

	m_curve.computeNaturalParameters(m_data.naturalParameters);
	m_curve.computeLengths(m_data.jointVolume, m_data.jointVolume, m_data.controlPoints, m_data.lengths, &m_data.naturalPoints);
	m_curve.computeArcLengthParameters(m_data.lengths, m_data.arcLengthParameters);
	m_curve.computeSplitLengthParameters(m_data.jointVolume, m_data.jointVolume, m_data.controlPoints, m_data.naturalPoints, m_data.lengths, 
		m_data.splitLengthParameters);

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

		// Each sequential reflection is stored as a composition of all previous reflections
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

		for (unsigned int i = 1; i < m_data.sampleCount; i++)
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

	if (m_data.isOrientEnabled)
	{
		m_data.isDrawRibbonEnabled = dataBlock.inputValue(drawRibbonAttr).asBool();
		m_data.counterTwistBlend = dataBlock.inputValue(counterTwistBlendAttr).asDouble();
		m_data.counterTwist = dataBlock.inputValue(counterTwistAttr).asAngle().asRadians();
		m_data.startTwist = dataBlock.inputValue(startTwistAttr).asAngle().asRadians();
		m_data.endTwist = dataBlock.inputValue(endTwistAttr).asAngle().asRadians();
		m_data.roll = dataBlock.inputValue(rollAttr).asAngle().asRadians();

		unsigned int increment = m_data.isDrawRibbonEnabled ? 1 : m_data.subdivisions + 1;
		unsigned int outputIndex = 0;

		for (unsigned int i = 0; i < m_data.sampleCount; i += increment)
		{
			bool isOutput = !m_data.isDrawRibbonEnabled || i % (m_data.subdivisions + 1) == 0;
			double totalWeightedTwist = 0.0;

			// End twist
			double endTwistWeight = m_data.naturalParameters[i];
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
					double twistAdjustmentWeight = twistAdjustment.curve.getValue(m_data.naturalParameters[i]);
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
					double scaleAdjustmentWeight = scaleAdjustment.curve.getValue(m_data.naturalParameters[i]);
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

			// Scale adjustment
			if (m_data.isScaleAdjustmentEnabled)
			{
				MVector vScaleAdjustment{ 1.0, 1.0, 1.0 };

				for (ScaleAdjustment& scaleAdjustment : m_data.scaleAdjustments)
				{
					double scaleAdjustmentWeight = scaleAdjustment.curve.getValue(m_data.naturalParameters[parameterIndex]);
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
void FlexiChainSingle::computeScaleAdjustments(MDataBlock& dataBlock)
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
void FlexiChainSingle::computeTwistAdjustments(MDataBlock& dataBlock)
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
	Calculates an equivalent natural parameter approximation for the given split-length parameter

	Args
	----
	splitLengthParameter = The split-length parameter from which to calculate the equivalent natural parameter, range = [0.0, 1.0]    */
double FlexiChainSingle::splitLengthToNaturalParameter(double splitLengthParameter)
{
	return m_curve.splitLengthToNaturalParameter(splitLengthParameter, m_data.jointVolume, m_data.jointVolume, 
		m_data.controlPoints, m_data.naturalPoints, m_data.lengths);
}

/*	Description
	-----------
	This function is used to calculate a value which represents the stability of the normal up-vector
	This up-vector is used to calculate the principal normal for the RMF calculation
	The output has a range of [0.0, 1.0], where higher values mean the RMF calculation becomes less stable
	This function will return success if orient data is being computed, failure otherwise    */
MStatus FlexiChainSingle::computeNormalStability(double& outStability)
{
	if (!m_data.isOrientEnabled)
		return MStatus::kFailure;

	MVector vLowerBoundTangent = m_data.controlPoints[1] - m_data.controlPoints[0];
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
MStatus FlexiChainSingle::computeCounterTwistStability(double& outStability)
{
	if (!m_data.isOrientEnabled)
		return MStatus::kFailure;

	MVector vUpperBoundTangent = m_data.controlPoints[3] - m_data.controlPoints[2];
	vUpperBoundTangent.normalize();
	outStability = m_data.vCounterTwistUp * vUpperBoundTangent;

	return MStatus::kSuccess;
}

/*	Description
	-----------
	This function is called exclusively by the FlexiChainSingleCounterTwist command and is used to calculate the counter twist cache for this node
	Counter twist is an iterative calculation which is designed to stabalise the end frame of the curve
	It essentially mitigates any twist deviation induced on the moving frame of the curve over a specified time range
	This function assumes the command from which it was called has validated all inputs and checked that orientation computation is enabled    */
MStatus FlexiChainSingle::computeCounterTwist(MTime startTime, MTime endTime, MTime timeStep, MObject& animCurveObj, MAnimCurveChange& animMod)
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
		MVector vUpperBoundTangent = m_data.controlPoints[3] - m_data.controlPoints[2];
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
	Samples the internal curve using previously evaluated data

	Args
	----
	t = Natural parameter of curve, range = [0,1]    */
MVector FlexiChainSingle::sampleCurve(double t) const
{
	return m_curve.sampleCurve(t, m_data.jointVolume, m_data.jointVolume, m_data.controlPoints);
}

/*	Description
	-----------
	Samples the first derivative of the internal curve using previously evaluated data
	The returned vector will not be normalized

	Args
	----
	t = Natural parameter of curve, range = [0,1]    */
MVector FlexiChainSingle::sampleFirstDerivative(double t) const
{
	return m_curve.sampleFirstDerivative(t, m_data.jointVolume, m_data.jointVolume, m_data.controlPoints);
}

void FlexiChainSingle::instancingChangedCallback(MDagPath& child, MDagPath& parent, void* clientData)
{
	MGlobal::displayWarning("FlexiChainSingleShape does not support instancing!");
}

const MRS::CubicTBezier& FlexiChainSingle::getCurve() const { return m_curve; }
const FlexiChainSingle::FlexiChainSingle_Data& FlexiChainSingle::getCurveData() const { return m_data; }
MDataBlock FlexiChainSingle::getDataBlock() { return forceCache(); }

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	The FlexiChainSingleCounterTwist command is used to calculate the counter twist cache for a given FlexiChainSingle node
	Counter twist is an iterative calculation which is designed to stabalise the end frame of the curve
	It essentially mitigates any twist deviation induced on the moving frame of the curve over a specified time range

	MEL Command
	-----------
	FlexiChainSingleCounterTwist [-startTime float] [-endTime float] [-timeStep float] [object]

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

FlexiChainSingle_CounterTwistCommand::FlexiChainSingle_CounterTwistCommand() : MPxCommand() {}
FlexiChainSingle_CounterTwistCommand::~FlexiChainSingle_CounterTwistCommand() {}

// ------ Registration ------

const char* FlexiChainSingle_CounterTwistCommand::kStartTimeFlag = "-st";
const char* FlexiChainSingle_CounterTwistCommand::kStartTimeFlagLong = "-startTime";
const char* FlexiChainSingle_CounterTwistCommand::kEndTimeFlag = "-et";
const char* FlexiChainSingle_CounterTwistCommand::kEndTimeFlagLong = "-endTime";
const char* FlexiChainSingle_CounterTwistCommand::kTimeStepFlag = "-ts";
const char* FlexiChainSingle_CounterTwistCommand::kTimeStepFlagLong = "-timeStep";

MSyntax FlexiChainSingle_CounterTwistCommand::newSyntax()
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
	"This command requires a single FlexiChainSingle or its transform node to be specified or selected."

#define kErrorInvalidType \
	"Object \"^1s\" has an invalid type. Only a FlexiChainSingle or its transform can be specified."

#define kErrorInvalidConnection \
	"Plug \"^1s.counterTwist\" has an incompatible destination connection from \"^2s\". Only animCurve connections will be overridden."

#define kErrorOrientationDisabled \
	"Plug \"^1s.computeOrientation\" holds false value. Ensure orientation is being computed by this node before proceeding."

bool FlexiChainSingle_CounterTwistCommand::isUndoable() const
{
	return true;
}

MStatus FlexiChainSingle_CounterTwistCommand::doIt(const MArgList& args)
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

	// Check if the selected plugin shape is a FlexiChainSingle node
	MDagPath pluginPath;
	iter.getDagPath(pluginPath);
	MFnDependencyNode fnDep{ pluginPath.node() };
	if (fnDep.typeId() != FlexiChainSingle::kTypeId)
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	FlexiChainSingle* locator = (FlexiChainSingle*)fnDep.userNode();

	// Check orient is enabled for the node
	// Note, orient computation is not a keyable attribute as its state must remain true during the counter-twist calculation
	MPlug orientEnabledPlug{ pluginPath.node(), FlexiChainSingle::computeOrientationAttr };
	if (!orientEnabledPlug.asBool())
	{
		MString msg;
		MString msgFormat = kErrorOrientationDisabled;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	// If an anim curve is connected, disconnect and delete it, otherwise error if the connection is not an anim curve
	MPlug counterTwistPlug{ pluginPath.node(), FlexiChainSingle::counterTwistAttr };
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

MStatus FlexiChainSingle_CounterTwistCommand::redoIt()
{
	MStatus status;

	status = m_animDGMod.doIt();
	if (status)
	{
		status = m_animChangeMod.redoIt();
	}

	return status;
}

MStatus FlexiChainSingle_CounterTwistCommand::undoIt()
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
	The FlexiChainSingleStability command is used to query the stability of the computed orient data
	It is designed for debugging purposes and is not undoable (ie. query only)
	Stability is effected by two calculations:
	- The up-vector used to calculate the principal normal for the RMF calculation
	- The counter-twist target vector which is used to iteratively calculate the total twist deviation of the end normal over a time interval

	MEL Command
	-----------
	FlexiChainSingleStability [-time float] [-normal] [-counterTwist] [object]

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

FlexiChainSingle_StabilityCommand::FlexiChainSingle_StabilityCommand() {}
FlexiChainSingle_StabilityCommand::~FlexiChainSingle_StabilityCommand() {}

// ------ Registration ------

const char* FlexiChainSingle_StabilityCommand::kTimeFlag = "-t";
const char* FlexiChainSingle_StabilityCommand::kTimeFlagLong = "-time";
const char* FlexiChainSingle_StabilityCommand::kNormalFlag = "-n";
const char* FlexiChainSingle_StabilityCommand::kNormalFlagLong = "-normal";
const char* FlexiChainSingle_StabilityCommand::kCounterTwistFlag = "-ct";
const char* FlexiChainSingle_StabilityCommand::kCounterTwistFlagLong = "-counterTwist";

MSyntax FlexiChainSingle_StabilityCommand::newSyntax()
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
	"This command requires a single FlexiChainSingle or its transform node to be specified or selected."

#define kErrorInvalidType \
	"Object \"^1s\" has an invalid type. Only a FlexiChainSingle or its transform can be specified."

#define kErrorOrientationDisabled \
	"Plug \"^1s.computeOrientation\" holds false value. Ensure orientation is being computed by this node before proceeding."

MStatus FlexiChainSingle_StabilityCommand::doIt(const MArgList& args)
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

	// Check if the selected plugin shape is a FlexiChainSingle node
	MDagPath pluginPath;
	iter.getDagPath(pluginPath);
	MFnDependencyNode fnDep{ pluginPath.node() };
	if (fnDep.typeId() != FlexiChainSingle::kTypeId)
	{
		MString msg;
		MString msgFormat = kErrorInvalidType;
		msg.format(msgFormat, path.partialPathName());
		displayError(msg);
		return MStatus::kFailure;
	}

	FlexiChainSingle* locator = (FlexiChainSingle*)fnDep.userNode();
	MDataBlock dataBlock = locator->_forceCache();

	// Check orient is enabled for the node
	bool isOrientEnabled = dataBlock.inputValue(FlexiChainSingle::computeOrientationAttr).asBool();
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
	dataBlock.inputValue(FlexiChainSingle::outputWorldFramesAttr);

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