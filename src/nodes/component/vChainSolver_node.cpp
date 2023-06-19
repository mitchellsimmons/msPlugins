#include "vChainSolver_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	A v-shaped chain forms a triangle given by two fixed length segments and a third variable length segment.
	The variable length segment is defined by the distance between the effector and the start of the chain.
	Once the effector offset has been calculated, the angles between each segment can be solved.
	The solver choice defines the relationship between the handle offset and the effector offset.

	The following diagram displays the shape of the solution in relation to segments s0 and s1:

				+	Chain Start
			   /|
		  s0  /0|
			 /  |
			/   |
		   /1   |  s2
			\   |
			 \  |
		  s1  \2|
			   \|
				+	Chain End (effector)
				|
				|
				+	Handle

	Attributes
	----------
	parentInverseFrame - matrix
		The frame used to localise the solution.
		The hierarchicalOutput attribute determines how the solution is localised relative to this frame (ie. sequential or flat hierarchy).
		In the general case, the user should provide the world inverse frame of the basis' parent transform.

	rootPosition - matrix
		A frame providing the world position of the root transform.

	handlePosition - matrix
		A frame providing the world position of the handle transform.

	upVectorPosition - matrix
		A frame providing the world position of the up-vector transform.

	aimAxis - enum
		The axis down which the aim-vector of the solution will be oriented.

		+x (0)
			The positive direction of the x-axis.
		+y (1)
			The positive direction of the y-axis.
		+z (2)
			The positive direction of the z-axis.
		-x (3)
			The negative direction of the x-axis.
		-y (4)
			The negative direction of the y-axis.
		-z (5)
			The negative direction of the z-axis.

	upAxis - enum
		The axis down which the up-vector of the solution will be oriented.
		This axis will be automatically resolved if it conflicts with the aimAxis.

		+x (0)
			The positive direction of the x-axis.
		+y (1)
			The positive direction of the y-axis.
		+z (2)
			The positive direction of the z-axis.
		-x (3)
			The negative direction of the x-axis.
		-y (4)
			The negative direction of the y-axis.
		-z (5)
			The negative direction of the z-axis.

	length0 - double [0.01, inf)
		The length of the first segment.

	length1 - double [0.01, inf)
		The length of the second segment.

	startToHandleOffset - double [0.0, inf)
		The distance between the start position and the handle position.

	direction - enum
		Specifies the direction of the chain.

		backward (-1)
			Angles will be negative.
		forward	 (1)
			Angles will be positive.

	solver - enum
		Specifies the type behaviour used to solve the chain lengths and angles.

		rigid (0)
			Enforces a rigid relationship between the handle and the effector for a period determined by the maxRigidExtensionRatio.
			Segments determined by length0 and length1 will maintain a fixed length during extension.
		lengthDampening	(1)
			Enforces a rigid relationship between the handle and the effector for a period determined by the maxRigidExtensionRatio.
			A lagging (non-linear) relationship is then enforced for a period determined by the maxNonRigidExtensionRatio.
			Segments determined by length0 and length1 will maintain a fixed length during extension.
		heightLockExtension	(2)
			Enforces a rigid relationship between the handle and the effector for a period determined by the maxRigidExtensionRatio + maxNonRigidExtensionRatio.
			Segments determined by length0 and length1 will maintain a fixed length during the period of extension determined by the maxRigidExtensionRatio.
			The segments will then extend for a period determined by the maxNonRigidExtensionRatio.
			As the segments extend, the rejection (height) of each segment will remain constant.
		heightReductionExtension (3)
			Enforces a rigid relationship between the handle and the effector for a period determined by the maxRigidExtensionRatio + maxNonRigidExtensionRatio.
			Segments determined by length0 and length1 will maintain a fixed length during the period of extension determined by the maxRigidExtensionRatio.
			The segments will then extend for a period determined by the maxNonRigidExtensionRatio.
			As the segments extend, the rejection (height) of each segment will be reduced by an amount determined by the maxHeightReductionRatio.

	maxCompressionRatio - double [0.0, 0.95]
		How far the chain can be compressed, as a percentage of the maximum rigid length.
		maxCompression = (length0 + length1) * maxCompressionRatio.
		If the maximum amount of compression is less than the maximum amount of rigid extension then compression will be clamped to rigid extension.

	maxRigidExtensionRatio - double [0.05, 1.0]
		How far the chain can be rigidly extended, as a percentage of the maximum rigid length.
		maxRigidExtension = (length0 + length1) * maxRigidExtensionRatio.
		For non-rigid solver types, this value should be set less than 1.0 (ie. to prevent maximum rigid extension, allowing for a transition to non-rigid extension).

	maxNonRigidExtensionRatio - double [0.0, inf)
		How far the chain can be non-rigidly extended, as a percentage of the maximum rigid length.
		maxNonRigidExtension = (length0 + length1) * maxNonRigidExtensionRatio.
		maxTotalExtension = (length0 + length1) * (maxRigidExtensionRatio + maxNonRigidExtensionRatio).
		This attribute has no effect on the rigid solver type.

	maxHeightReductionRatio - double [0.0, 1.0]
		How far the height will be reduced at maximum non-rigid extension, as a percentage of the rigid height at maximum rigid extension.
		This attribute only affects the heightReductionExtension solver.
		A value of 0.0 will produce the same behaviour as the heightLockExtension solver (ie. 0% reduction in rigid height at maximum non-rigid extension).
		A value of 1.0 will ensure the chain is completely straight at the point of maximum non-rigid extension (ie. 100% reduction in rigid height at maximum non-rigid extension).

	heightReductionDeceleration - double [0.0, 0.95]
		Specifies the rate of change in height when the heightReductionExtension solver choice is selected.
		Large values correspond to higher amounts of deceleration (ie. quicker falloff).
		A value of zero corresponds to zero deceleration (ie. linear falloff).
		This attribute should be used to smooth the transition between rigid and non-rigid extension.
		For small amounts of non-rigid extension, a linear falloff will usually produce a smooth transition.
		For large amounts of non-rigid extension, a faster falloff will help produce a smooth transition.

	hierarchicalOutput - bool
		Whether output frames should have sequential locality or a shared locality (ie. flat hierarchy).
		In the case of sequential locality, each frame will be localised to the previous frame, whilst the initial frame will be localised using the parentInverseFrame.
		In the case of shared locality, all frames will be localised using the parentInverseFrame.

	outputFrames - matrixArray
		Array of matrices forming the v-chain solution.
		The locality of frames is determined by the hierarchicalOutput and parentInverseFrame attributes.
		Of special importance is the initial frame, representing the basis for the solution, as well as the end frame which is re-oriented to the basis.

	outputRootToEffectorOffset - double
		The distance between the root position and the effector position.

	Notes
	-----
	We are tracking the dirty state of input plugs in order to optimize for the general case where all outputs are connected.
	This means computation for all outputs occurs in a single pass instead of re-computing data for each dirty output seperately.
	The tracking method is based upon the "simpleEvaluationNode" from the devkit examples.
	https://around-the-corner.typepad.com/adn/2017/05/improve-performance-with-mpxnodepreevaluation-mpxnodepostevaluation.html

	Relative chain length ratios have been used to define limits on compression and extension.
	Relative limits are additive instead of absolute, meaning each ratio is independent of the other.
	This avoids the potential issue of absolute ratios having overlapping boundaries.

	When the heightReductionExtension solver is selected, the user should take care to ensure the height of the non-rigid solution does not reduce too quickly.
	The non-rigid lengths may shrink as a result of excessive height reduction.
	Excessive height reduction may occur when the maxRigidExtensionRatio/maxNonRigidExtensionRatio are too small or the heightReductionDeceleration is too large.
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

VChainSolver::Triangle::Triangle() :
	a{ 0.0 },
	b{ 0.0 },
	c{ 0.0 },
	A{ 0.0 },
	B{ 0.0 },
	C{ 0.0 }
{}

VChainSolver::Triangle::~Triangle() {}

VChainSolver::VChainSolver() :
	m_evalSinceDirty{ false },
	m_outputFrames(7)
{}

VChainSolver::~VChainSolver() {}

// ------ Attr ------
MObject VChainSolver::parentInverseFrameAttr;
MObject VChainSolver::rootPositionAttr;
MObject VChainSolver::handlePositionAttr;
MObject VChainSolver::upVectorPositionAttr;
MObject VChainSolver::aimAxisAttr;
MObject VChainSolver::upAxisAttr;
MObject VChainSolver::length0Attr;
MObject VChainSolver::length1Attr;
MObject VChainSolver::directionAttr;
MObject VChainSolver::solverAttr;
MObject VChainSolver::maxCompressionRatioAttr;
MObject VChainSolver::maxRigidExtensionRatioAttr;
MObject VChainSolver::maxNonRigidExtensionRatioAttr;
MObject VChainSolver::maxHeightReductionRatioAttr;
MObject VChainSolver::heightReductionDecelerationAttr;
MObject VChainSolver::hierarchicalOutputAttr;
MObject VChainSolver::outputFramesAttr;
MObject VChainSolver::outputRootToEffectorOffsetAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VChainSolver::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VChainSolver::initialize()
{
	std::unordered_map<const char*, short> axisFields{ {"+x", kPosX}, {"+y", kPosY}, {"+z", kPosZ}, {"-x", kNegX}, {"-y", kNegY}, {"-z", kNegZ} };
	std::unordered_map<const char*, short> directionFields{ {"backward", kBackward}, {"forward", kForward} };
	std::unordered_map<const char*, short> solverFields{ {"rigid", kRigidSolver}, {"lengthDampening", kLengthDampeningSolver},
		{"heightLockExtension", kHeightLockExtensionSolver}, {"heightReductionExtension", kHeightReductionExtensionSolver} };
	std::vector<MMatrix> outputFrames;

	createMatrixAttribute(parentInverseFrameAttr, "parentInverseFrame", "parentInverseFrame", MMatrix::identity, kDefaultPreset);
	createMatrixAttribute(rootPositionAttr, "rootPosition", "rootPosition", MMatrix::identity, kDefaultPreset);
	createMatrixAttribute(handlePositionAttr, "handlePosition", "handlePosition", MMatrix::identity, kDefaultPreset);
	createMatrixAttribute(upVectorPositionAttr, "upVectorPosition", "upVectorPosition", MMatrix::identity, kDefaultPreset);
	createEnumAttribute(aimAxisAttr, "aimAxis", "aimAxis", axisFields, 0, kDefaultPreset | kKeyable);
	createEnumAttribute(upAxisAttr, "upAxis", "upAxis", axisFields, 1, kDefaultPreset | kKeyable);
	createDoubleAttribute(length0Attr, "length0", "length0", 10.0, kDefaultPreset | kKeyable);
	setMin<double>(length0Attr, 0.01);
	createDoubleAttribute(length1Attr, "length1", "length1", 10.0, kDefaultPreset | kKeyable);
	setMin<double>(length1Attr, 0.01);
	createEnumAttribute(directionAttr, "direction", "direction", directionFields, 1, kDefaultPreset | kKeyable);
	createEnumAttribute(solverAttr, "solver", "solver", solverFields, 0, kDefaultPreset | kKeyable);
	createDoubleAttribute(maxCompressionRatioAttr, "maxCompressionRatio", "maxCompressionRatio", 0.9, kDefaultPreset | kKeyable);
	setMinMax<double>(maxCompressionRatioAttr, 0.0, 0.95);
	createDoubleAttribute(maxRigidExtensionRatioAttr, "maxRigidExtensionRatio", "maxRigidExtensionRatio", 0.98, kDefaultPreset | kKeyable);
	setMinMax<double>(maxRigidExtensionRatioAttr, 0.05, 1.0);
	createDoubleAttribute(maxNonRigidExtensionRatioAttr, "maxNonRigidExtensionRatio", "maxNonRigidExtensionRatio", 0.05, kDefaultPreset | kKeyable);
	setMin<double>(maxNonRigidExtensionRatioAttr, 0.0);
	createDoubleAttribute(maxHeightReductionRatioAttr, "maxHeightReductionRatio", "maxHeightReductionRatio", 1.0, kDefaultPreset | kKeyable);
	setMinMax<double>(maxHeightReductionRatioAttr, 0.0, 1.0);
	createDoubleAttribute(heightReductionDecelerationAttr, "heightReductionDeceleration", "heightReductionDeceleration", 0.0, kDefaultPreset | kKeyable);
	setMinMax<double>(heightReductionDecelerationAttr, 0.0, 0.95);
	createBoolAttribute(hierarchicalOutputAttr, "hierarchicalOutput", "hierarchicalOutput", true, kDefaultPreset | kKeyable);
	createMatrixDataArrayAttribute(outputFramesAttr, "outputFrames", "outputFrames", outputFrames, kReadOnlyPreset);
	createDoubleAttribute(outputRootToEffectorOffsetAttr, "outputRootToEffectorOffset", "outputRootToEffectorOffset", 10.0, kReadOnlyPreset);

	addAttribute(parentInverseFrameAttr);
	addAttribute(rootPositionAttr);
	addAttribute(handlePositionAttr);
	addAttribute(upVectorPositionAttr);
	addAttribute(aimAxisAttr);
	addAttribute(upAxisAttr);
	addAttribute(length0Attr);
	addAttribute(length1Attr);
	addAttribute(directionAttr);
	addAttribute(solverAttr);
	addAttribute(maxCompressionRatioAttr);
	addAttribute(maxRigidExtensionRatioAttr);
	addAttribute(maxNonRigidExtensionRatioAttr);
	addAttribute(maxHeightReductionRatioAttr);
	addAttribute(heightReductionDecelerationAttr);
	addAttribute(hierarchicalOutputAttr);
	addAttribute(outputFramesAttr);
	addAttribute(outputRootToEffectorOffsetAttr);

	attributeAffects(parentInverseFrameAttr, outputFramesAttr);
	attributeAffects(rootPositionAttr, outputFramesAttr);
	attributeAffects(handlePositionAttr, outputFramesAttr);
	attributeAffects(upVectorPositionAttr, outputFramesAttr);
	attributeAffects(aimAxisAttr, outputFramesAttr);
	attributeAffects(upAxisAttr, outputFramesAttr);
	attributeAffects(length0Attr, outputFramesAttr);
	attributeAffects(length1Attr, outputFramesAttr);
	attributeAffects(directionAttr, outputFramesAttr);
	attributeAffects(solverAttr, outputFramesAttr);
	attributeAffects(maxCompressionRatioAttr, outputFramesAttr);
	attributeAffects(maxRigidExtensionRatioAttr, outputFramesAttr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputFramesAttr);
	attributeAffects(maxHeightReductionRatioAttr, outputFramesAttr);
	attributeAffects(heightReductionDecelerationAttr, outputFramesAttr);
	attributeAffects(hierarchicalOutputAttr, outputFramesAttr);

	attributeAffects(parentInverseFrameAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(rootPositionAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(handlePositionAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(upVectorPositionAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(aimAxisAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(upAxisAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(length0Attr, outputRootToEffectorOffsetAttr);
	attributeAffects(length1Attr, outputRootToEffectorOffsetAttr);
	attributeAffects(directionAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(solverAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(maxCompressionRatioAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(maxRigidExtensionRatioAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(maxHeightReductionRatioAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(heightReductionDecelerationAttr, outputRootToEffectorOffsetAttr);
	attributeAffects(hierarchicalOutputAttr, outputRootToEffectorOffsetAttr);

	return MStatus::kSuccess;
}

MStatus VChainSolver::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	if (plug == parentInverseFrameAttr || plug == rootPositionAttr || plug == handlePositionAttr || plug == upVectorPositionAttr ||
		plug == aimAxisAttr || plug == upAxisAttr || plug == length0Attr || plug == length1Attr || plug == directionAttr ||
		plug == solverAttr || plug == maxCompressionRatioAttr || plug == maxRigidExtensionRatioAttr || plug == maxNonRigidExtensionRatioAttr ||
		plug == maxHeightReductionRatioAttr || plug == heightReductionDecelerationAttr || plug == hierarchicalOutputAttr)
		m_evalSinceDirty = false;

	return MStatus::kSuccess;
}

MStatus VChainSolver::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
{
	MStatus status;

	// We only signal evaluation in a normal context
	if (!context.isNormal())
		return MStatus::kFailure;

	if ((evaluationNode.dirtyPlugExists(parentInverseFrameAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(rootPositionAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(handlePositionAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(upVectorPositionAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(aimAxisAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(upAxisAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(length0Attr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(length1Attr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(directionAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(solverAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxCompressionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxRigidExtensionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxNonRigidExtensionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxHeightReductionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(heightReductionDecelerationAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(hierarchicalOutputAttr, &status) && status))
	{
		m_evalSinceDirty = false;
	}

	return MS::kSuccess;
}

MStatus VChainSolver::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputFramesAttr)
		return MStatus::kUnknownParameter;

	// ------ Evaluation ------
	if (!m_evalSinceDirty)
		computeData(dataBlock);

	// ------ Output ------
	outputMatrixDataArrayValue(dataBlock, outputFramesAttr, m_outputFrames);
	outputDoubleValue(dataBlock, outputRootToEffectorOffsetAttr, m_solution.c);

	return MStatus::kSuccess;
}

void VChainSolver::computeData(MDataBlock& dataBlock)
{
	// ------ Basis ------
	MMatrix parentInverseFrame = inputMatrixValue(dataBlock, parentInverseFrameAttr);
	MMatrix rootFrame = inputMatrixValue(dataBlock, rootPositionAttr);
	MVector rootPosition = MRS::extractTranslation(rootFrame);
	MMatrix handleFrame = inputMatrixValue(dataBlock, handlePositionAttr);
	MVector handlePosition = MRS::extractTranslation(handleFrame);
	MMatrix upVectorFrame = inputMatrixValue(dataBlock, upVectorPositionAttr);
	MVector upVectorPosition = MRS::extractTranslation(upVectorFrame);
	short aimAxis = inputEnumValue(dataBlock, aimAxisAttr);
	short upAxis = inputEnumValue(dataBlock, upAxisAttr);

	// Using double cross product technique to build an orthonormal transform
	MVector aimVector = handlePosition - rootPosition;
	double handleOffset = aimVector.length();
	aimVector.normalize();

	MVector binormalVector = aimVector ^ (upVectorPosition - rootPosition);
	binormalVector.normalize();

	MVector normalVector = binormalVector ^ aimVector;

	MRS::Matrix33<double> basisOrientFrame{ aimVector, normalVector , binormalVector };

	// Resolve upAxis conflicts
	switch (aimAxis)
	{
		// upAxis defaults to (+y) if it conflicts with aimAxis (+x, -x)
		case kPosX: case kNegX:
		{
			if (upAxis == kPosX || upAxis == kNegX)
				upAxis = kPosY;

			break;
		}
		// upAxis defaults to (+x) if it conflicts with aimAxis (+y, -y)
		case kPosY: case kNegY:
		{
			if (upAxis == kPosY || upAxis == kNegY)
				upAxis = kPosX;

			break;
		}
		// upAxis defaults to (+y) if it conflicts with aimAxis (+z, -z)
		case kPosZ: case kNegZ:
		{
			if (upAxis == kPosZ || upAxis == kNegZ)
				upAxis = kPosY;

			break;
		}
	}

	// Determine the roll axis
	short aimX = aimAxis == kPosX ? 1 : aimAxis == kNegX ? -1 : 0;
	short upX = upAxis == kPosX ? 1 : upAxis == kNegX ? -1 : 0;
	short aimY = aimAxis == kPosY ? 1 : aimAxis == kNegY ? -1 : 0;
	short upY = upAxis == kPosY ? 1 : upAxis == kNegY ? -1 : 0;
	short aimZ = aimAxis == kPosZ ? 1 : aimAxis == kNegZ ? -1 : 0;
	short upZ = upAxis == kPosZ ? 1 : upAxis == kNegZ ? -1 : 0;

	// Cross product
	short rollX = aimY * upZ - aimZ * upY;
	short rollY = aimZ * upX - aimX * upZ;
	short rollZ = aimX * upY - aimY * upX;

	Axis rollAxis = rollX == 1 ? kPosX : rollX == -1 ? kNegX : rollY == 1 ? kPosY : rollY == -1 ? kNegY : rollZ == 1 ? kPosZ : kNegZ;

	// Re-orient the basis
	switch (aimAxis)
	{
		case kPosX:
		{
			switch (upAxis)
			{
				case kPosZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(-M_PI_2);
					break;
				}
				case kNegY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI);
					break;
				}
				case kNegZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI_2);
					break;
				}
			}
			break;
		}
		case kPosY:
		{
			switch (upAxis)
			{
				case kPosX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kPosZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
			}
			break;
		}
		case kPosZ:
		{
			switch (upAxis)
			{
				case kPosX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI_2);
					break;
				}
				case kNegX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
		case kNegX:
		{
			switch (upAxis)
			{
				case kPosY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI);
					break;
				}
				case kPosZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI);
					break;
				}
				case kNegY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI);
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI);
					break;
				}
				case kNegZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
		case kNegY:
		{
			switch (upAxis)
			{
				case kPosX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kNegX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(M_PI);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegZ:
				{
					basisOrientFrame = basisOrientFrame.preRotateInX(M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI_2);
					break;
				}
			}
			break;
		}
		case kNegZ:
		{
			switch (upAxis)
			{
				case kPosX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI_2);
					break;
				}
				case kPosY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(-M_PI_2);
					break;
				}
				case kNegX:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(-M_PI_2);
					break;
				}
				case kNegY:
				{
					basisOrientFrame = basisOrientFrame.preRotateInY(-M_PI_2);
					basisOrientFrame = basisOrientFrame.preRotateInZ(M_PI);
					break;
				}
			}
			break;
		}
	}

	MMatrix basisFrame;
	basisOrientFrame.get(basisFrame.matrix);
	basisFrame[3][0] = rootPosition[0]; basisFrame[3][1] = rootPosition[1]; basisFrame[3][2] = rootPosition[2];
	basisFrame *= parentInverseFrame;

	// ------ Triangle ------
	double rigidA = inputDoubleValue(dataBlock, length0Attr);
	double rigidB = inputDoubleValue(dataBlock, length1Attr);
	short direction = inputEnumValue(dataBlock, directionAttr);
	short solver = inputEnumValue(dataBlock, solverAttr);
	double maxCompressionRatio = inputDoubleValue(dataBlock, maxCompressionRatioAttr);
	double maxRigidExtensionRatio = inputDoubleValue(dataBlock, maxRigidExtensionRatioAttr);
	double maxNonRigidExtensionRatio = inputDoubleValue(dataBlock, maxNonRigidExtensionRatioAttr);
	// Ensure compression is less than extension
	maxCompressionRatio = 1.0 - maxCompressionRatio < maxRigidExtensionRatio ? maxCompressionRatio : 1.0 - maxRigidExtensionRatio;

	double chainLength = rigidA + rigidB;
	double minEffectorOffset = chainLength * (1.0 - maxCompressionRatio);
	double maxRigidEffectorOffset = chainLength * maxRigidExtensionRatio;
	double maxNonRigidEffectorOffset = chainLength * (maxRigidExtensionRatio + maxNonRigidExtensionRatio);

	// Clamp the effector between the maximum compression and maximum rigid extension
	double effectorOffset = std::min(std::max(handleOffset, minEffectorOffset), maxRigidEffectorOffset);

	// Rigid lengths
	m_solution.a = rigidA;
	m_solution.b = rigidB;
	m_solution.c = effectorOffset;

	// In some cases it is optimal to solve lengths and angles together
	bool solvedAngles = false;

	// Non-rigid lengths and angles
	if (handleOffset > maxRigidEffectorOffset)
	{
		switch (solver)
		{
			case kRigidSolver:
				break;

				// https://www.desmos.com/calculator/x6csxjiqtn
			case kLengthDampeningSolver:
			{
				// Length dampening only applies if the rigid chain is not fully extended
				if (maxRigidExtensionRatio < 1.0)
				{
					double softness = chainLength - maxRigidEffectorOffset;
					effectorOffset = std::min(-softness * std::pow(M_E, (maxRigidEffectorOffset - handleOffset) / softness) + chainLength, maxNonRigidEffectorOffset);
					m_solution.c = effectorOffset;
				}

				break;
			}

			case kHeightLockExtensionSolver:
			{
				effectorOffset = std::min(handleOffset, maxNonRigidEffectorOffset);
				double cosRigidA = (rigidB * rigidB + maxRigidEffectorOffset * maxRigidEffectorOffset - rigidA * rigidA) / (2 * rigidB * maxRigidEffectorOffset);
				double cosRigidB = (rigidA * rigidA + maxRigidEffectorOffset * maxRigidEffectorOffset - rigidB * rigidB) / (2 * rigidA * maxRigidEffectorOffset);
				double projRigidA = cosRigidB * rigidA;
				double projRigidB = cosRigidA * rigidB;
				double ratioA = projRigidA / (projRigidA + projRigidB);
				double projNonRigidA = ratioA * effectorOffset;
				double projNonRigidB = effectorOffset - projNonRigidA;
				double height = std::sqrt(rigidA * rigidA - projRigidA * projRigidA);
				double heightSquared = height * height;
				double nonRigidA = std::sqrt(projNonRigidA * projNonRigidA + heightSquared);
				double nonRigidB = std::sqrt(projNonRigidB * projNonRigidB + heightSquared);

				m_solution.a = nonRigidA;
				m_solution.b = nonRigidB;
				m_solution.c = effectorOffset;

				// Optimisation - solve angles using pre-calculated data
				if (MRS::isEqual(nonRigidA, nonRigidB))
					m_solution.A = m_solution.B = std::atan(height / projNonRigidA);
				else
				{
					m_solution.A = std::atan(height / projNonRigidB);
					m_solution.B = std::atan(height / projNonRigidA);
				}

				m_solution.C = M_PI - m_solution.A - m_solution.B;
				solvedAngles = true;

				break;
			}

			case kHeightReductionExtensionSolver:
			{
				double heightReductionDeceleration = inputDoubleValue(dataBlock, heightReductionDecelerationAttr);
				double maxHeightReductionRatio = inputDoubleValue(dataBlock, maxHeightReductionRatioAttr);

				effectorOffset = std::min(handleOffset, maxNonRigidEffectorOffset);
				double nonRigidRatio = (effectorOffset - maxRigidEffectorOffset) / (maxNonRigidEffectorOffset - maxRigidEffectorOffset);
				double cosRigidA = (rigidB * rigidB + maxRigidEffectorOffset * maxRigidEffectorOffset - rigidA * rigidA) / (2 * rigidB * maxRigidEffectorOffset);
				double cosRigidB = (rigidA * rigidA + maxRigidEffectorOffset * maxRigidEffectorOffset - rigidB * rigidB) / (2 * rigidA * maxRigidEffectorOffset);
				double projRigidA = cosRigidB * rigidA;
				double projRigidB = cosRigidA * rigidB;
				double ratioA = projRigidA / (projRigidA + projRigidB);
				double projNonRigidA = ratioA * effectorOffset;
				double projNonRigidB = effectorOffset - projNonRigidA;
				double rigidHeight = std::sqrt(rigidA * rigidA - projRigidA * projRigidA);
				double minNonRigidHeight = rigidHeight * (1.0 - maxHeightReductionRatio);
				double nonRigidHeight = maxHeightReductionRatio == 0.0 ? rigidHeight :
					MRS::variableDeceleration(rigidHeight, minNonRigidHeight, nonRigidRatio, heightReductionDeceleration);
				double nonRigidHeightSquared = nonRigidHeight * nonRigidHeight;
				double nonRigidA = std::sqrt(projNonRigidA * projNonRigidA + nonRigidHeightSquared);
				double nonRigidB = std::sqrt(projNonRigidB * projNonRigidB + nonRigidHeightSquared);

				m_solution.a = nonRigidA;
				m_solution.b = nonRigidB;
				m_solution.c = effectorOffset;

				// Optimisation - solve angles using pre-calculated data
				if (MRS::isEqual(nonRigidA, nonRigidB))
					m_solution.A = m_solution.B = std::atan(nonRigidHeight / projNonRigidA);
				else
				{
					m_solution.A = std::atan(nonRigidHeight / projNonRigidB);
					m_solution.B = std::atan(nonRigidHeight / projNonRigidA);
				}

				m_solution.C = M_PI - m_solution.A - m_solution.B;
				solvedAngles = true;

				break;
			}
		}
	}

	// Solve angles
	if (!solvedAngles)
		solveAngles(m_solution);

	// Adjust direction
	m_solution.A *= direction;
	m_solution.B *= direction;
	m_solution.C *= direction;

	// ------ Frames ------
	bool hierarchicalOutput = inputBoolValue(dataBlock, hierarchicalOutputAttr);

	MRS::Matrix44<double> localFrame0;
	MRS::Matrix44<double> localFrame1;
	MRS::Matrix44<double> localFrame2;
	MRS::Matrix44<double> localFrame3;
	MRS::Matrix44<double> localFrame4;
	MRS::Matrix44<double> localFrame5;

	// Hierarchical ouputs have sequential locality. The initial output has locality relative to the basis parent frame (ie. determined via parentInverseFrame)
	if (hierarchicalOutput)
	{
		switch (rollAxis)
		{
			case kPosX:
			{
				localFrame0 = localFrame0.preRotateInX(m_solution.B);

				switch (aimAxis)
				{
					case kPosY:
					{
						localFrame1[3][1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						localFrame1[3][1] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						localFrame1[3][2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						localFrame1[3][2] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInX((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame3.preRotateInX((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosY:
					{
						localFrame4[3][1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						localFrame4[3][1] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						localFrame4[3][2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						localFrame4[3][2] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInX(m_solution.A);

				break;
			}
			case kPosY:
			{
				localFrame0 = localFrame0.preRotateInY(m_solution.B);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame1[3][0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						localFrame1[3][0] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						localFrame1[3][2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						localFrame1[3][2] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInY((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame3.preRotateInY((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame4[3][0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						localFrame4[3][0] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						localFrame4[3][2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						localFrame4[3][2] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInY(m_solution.A);

				break;
			}
			case kPosZ:
			{
				localFrame0 = localFrame0.preRotateInZ(m_solution.B);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame1[3][0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						localFrame1[3][0] = -m_solution.a;
						break;
					}
					case kPosY:
					{
						localFrame1[3][1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						localFrame1[3][1] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInZ((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame3.preRotateInZ((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame4[3][0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						localFrame4[3][0] = -m_solution.b;
						break;
					}
					case kPosY:
					{
						localFrame4[3][1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						localFrame4[3][1] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInZ(m_solution.A);

				break;
			}
			case kNegX:
			{
				localFrame0 = localFrame0.preRotateInX(-m_solution.B);

				switch (aimAxis)
				{
					case kPosY:
					{
						localFrame1[3][1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						localFrame1[3][1] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						localFrame1[3][2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						localFrame1[3][2] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInX(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame3.preRotateInX(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosY:
					{
						localFrame4[3][1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						localFrame4[3][1] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						localFrame4[3][2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						localFrame4[3][2] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInX(-m_solution.A);

				break;
			}
			case kNegY:
			{
				localFrame0 = localFrame0.preRotateInY(-m_solution.B);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame1[3][0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						localFrame1[3][0] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						localFrame1[3][2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						localFrame1[3][2] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInY(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame3.preRotateInY(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame4[3][0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						localFrame4[3][0] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						localFrame4[3][2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						localFrame4[3][2] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInY(-m_solution.A);

				break;
			}
			case kNegZ:
			{
				localFrame0 = localFrame0.preRotateInZ(-m_solution.B);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame1[3][0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						localFrame1[3][0] = -m_solution.a;
						break;
					}
					case kPosY:
					{
						localFrame1[3][1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						localFrame1[3][1] = -m_solution.a;
						break;
					}
				}

				localFrame2 = localFrame2.preRotateInZ(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame3.preRotateInZ(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						localFrame4[3][0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						localFrame4[3][0] = -m_solution.b;
						break;
					}
					case kPosY:
					{
						localFrame4[3][1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						localFrame4[3][1] = -m_solution.b;
						break;
					}
				}

				localFrame5 = localFrame5.preRotateInZ(-m_solution.A);

				break;
			}
		}
	}
	// Non-hierarchical outputs all have locality relative to the basis parent frame (ie. determined via parentInverseFrame)
	else
	{
		double aOffset[3]{ 0.0, 0.0, 0.0 };
		double bOffset[3]{ 0.0, 0.0, 0.0 };

		switch (rollAxis)
		{
			case kPosX:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInX(m_solution.B);
				
				switch (aimAxis)
				{
					case kPosY:
					{
						aOffset[1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						aOffset[1] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						aOffset[2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						aOffset[2] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInX((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame2.preRotateInX((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosY:
					{
						bOffset[1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						bOffset[1] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						bOffset[2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						bOffset[2] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInX(m_solution.A);

				break;
			}
			case kPosY:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInY(m_solution.B);
				
				switch (aimAxis)
				{
					case kPosX:
					{
						aOffset[0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						aOffset[0] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						aOffset[2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						aOffset[2] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInY((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame2.preRotateInY((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						bOffset[0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						bOffset[0] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						bOffset[2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						bOffset[2] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInY(m_solution.A);

				break;
			}
			case kPosZ:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInZ(m_solution.B);

				switch (aimAxis)
				{
					case kPosX:
					{
						aOffset[0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						aOffset[0] = -m_solution.a;
						break;
					}
					case kPosY:
					{
						aOffset[1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						aOffset[1] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInZ((m_solution.C - M_PI * direction) * 0.5);

				localFrame3 = localFrame2.preRotateInZ((m_solution.C - M_PI * direction) * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						bOffset[0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						bOffset[0] = -m_solution.b;
						break;
					}
					case kPosY:
					{
						bOffset[1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						bOffset[1] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInZ(m_solution.A);

				break;
			}
			case kNegX:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInX(-m_solution.B);
				
				switch (aimAxis)
				{
					case kPosY:
					{
						aOffset[1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						aOffset[1] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						aOffset[2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						aOffset[2] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInX(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame2.preRotateInX(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosY:
					{
						bOffset[1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						bOffset[1] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						bOffset[2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						bOffset[2] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInX(-m_solution.A);

				break;
			}
			case kNegY:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInY(-m_solution.B);
				
				switch (aimAxis)
				{
					case kPosX:
					{
						aOffset[0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						aOffset[0] = -m_solution.a;
						break;
					}
					case kPosZ:
					{
						aOffset[2] = m_solution.a;
						break;
					}
					case kNegZ:
					{
						aOffset[2] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInY(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame2.preRotateInY(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						bOffset[0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						bOffset[0] = -m_solution.b;
						break;
					}
					case kPosZ:
					{
						bOffset[2] = m_solution.b;
						break;
					}
					case kNegZ:
					{
						bOffset[2] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInY(-m_solution.A);

				break;
			}
			case kNegZ:
			{
				localFrame0 = basisFrame.matrix;
				localFrame0 = localFrame0.preRotateInZ(-m_solution.B);
				
				switch (aimAxis)
				{
					case kPosX:
					{
						aOffset[0] = m_solution.a;
						break;
					}
					case kNegX:
					{
						aOffset[0] = -m_solution.a;
						break;
					}
					case kPosY:
					{
						aOffset[1] = m_solution.a;
						break;
					}
					case kNegY:
					{
						aOffset[1] = -m_solution.a;
						break;
					}
				}

				localFrame1 = localFrame0.preTranslate(aOffset);

				localFrame2 = localFrame1.preRotateInZ(M_PI_2 * direction - m_solution.C * 0.5);

				localFrame3 = localFrame2.preRotateInZ(M_PI_2 * direction - m_solution.C * 0.5);

				switch (aimAxis)
				{
					case kPosX:
					{
						bOffset[0] = m_solution.b;
						break;
					}
					case kNegX:
					{
						bOffset[0] = -m_solution.b;
						break;
					}
					case kPosY:
					{
						bOffset[1] = m_solution.b;
						break;
					}
					case kNegY:
					{
						bOffset[1] = -m_solution.b;
						break;
					}
				}

				localFrame4 = localFrame3.preTranslate(bOffset);

				localFrame5 = localFrame4.preRotateInZ(-m_solution.A);

				break;
			}
		}
	}

	m_outputFrames[0] = basisFrame;
	localFrame0.get(m_outputFrames[1].matrix);
	localFrame1.get(m_outputFrames[2].matrix);
	localFrame2.get(m_outputFrames[3].matrix);
	localFrame3.get(m_outputFrames[4].matrix);
	localFrame4.get(m_outputFrames[5].matrix);
	localFrame5.get(m_outputFrames[6].matrix);

	// ------ Internal State ------
	m_evalSinceDirty = true;
}

// ------ Helpers ------

/*	Solve the angles of a given Triangle whose sides are assumed to be non-zero	lengths */
void VChainSolver::solveAngles(Triangle& triangle)
{
	assert(triangle.a > 0.0);
	assert(triangle.b > 0.0);
	assert(triangle.c > 0.0);

	double aSquared = triangle.a * triangle.a;
	double bSquared = triangle.b * triangle.b;
	double cSquared = triangle.c * triangle.c;

	triangle.C = std::acos((aSquared + bSquared - cSquared) / (2 * triangle.a * triangle.b));

	// Optimization for isosceles
	if (MRS::isEqual(triangle.a, triangle.b))
		triangle.A = triangle.B = (M_PI - triangle.C) / 2.0;
	else
	{
		triangle.A = std::acos((bSquared + cSquared - aSquared) / (2 * triangle.b * triangle.c));
		triangle.B = M_PI - triangle.A - triangle.C;
	}
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------