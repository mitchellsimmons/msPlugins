#include "vChainPlanarSolver_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	A v-shaped chain forms a triangle given by two fixed length segments and a third variable length segment.
	The variable length segment is defined by the distance between the effector and the start of the chain.
	Once the effector offset has been calculated, the angles between each segment can be solved.
	The solver choice defines the relationship between the handle offset and the effector offset.

	The following diagram displays the output angles in relation to segments s0 and s1:

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

	outputAngle0 - doubleAngle
		The angle formed between the first segment and the chain start to chain end segment.

	outputAngle1 - doubleAngle
		The angle formed between the first segment and the second segment.

	outputAngle2 - doubleAngle	
		The angle formed between the second segment and the chain start to chain end segment.

	outputLength0 - double
		The length of the first segment.

	outputLength1 - double
		The length of the second segment.

	outputStartToEffectorOffset - double
		The distance between the start position and the effector position.

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

VChainPlanarSolver::Triangle::Triangle() :
	a{ 0.0 },
	b{ 0.0 },
	c{ 0.0 },
	A{ 0.0 },
	B{ 0.0 },
	C{ 0.0 }
{}

VChainPlanarSolver::Triangle::~Triangle() {}

VChainPlanarSolver::VChainPlanarSolver() :
	m_evalSinceDirty{ false }
{}

VChainPlanarSolver::~VChainPlanarSolver() {}

// ------ Attr ------
MObject VChainPlanarSolver::length0Attr;
MObject VChainPlanarSolver::length1Attr;
MObject VChainPlanarSolver::startToHandleOffsetAttr;
MObject VChainPlanarSolver::directionAttr;
MObject VChainPlanarSolver::solverAttr;
MObject VChainPlanarSolver::maxCompressionRatioAttr;
MObject VChainPlanarSolver::maxRigidExtensionRatioAttr;
MObject VChainPlanarSolver::maxNonRigidExtensionRatioAttr;
MObject VChainPlanarSolver::maxHeightReductionRatioAttr;
MObject VChainPlanarSolver::heightReductionDecelerationAttr;
MObject VChainPlanarSolver::outputAngle0Attr;
MObject VChainPlanarSolver::outputAngle1Attr;
MObject VChainPlanarSolver::outputAngle2Attr;
MObject VChainPlanarSolver::outputLength0Attr;
MObject VChainPlanarSolver::outputLength1Attr;
MObject VChainPlanarSolver::outputStartToEffectorOffsetAttr;

// ------ MPxNode ------
MPxNode::SchedulingType VChainPlanarSolver::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus VChainPlanarSolver::initialize()
{
	std::unordered_map<const char*, short> directionFields{ {"backward", kBackward}, {"forward", kForward} };
	std::unordered_map<const char*, short> solverFields{ {"rigid", kRigidSolver}, {"lengthDampening", kLengthDampeningSolver},
		{"heightLockExtension", kHeightLockExtensionSolver}, {"heightReductionExtension", kHeightReductionExtensionSolver} };

	createDoubleAttribute(length0Attr, "length0", "length0", 10.0, kDefaultPreset | kKeyable);
	setMin<double>(length0Attr, 0.01);
	createDoubleAttribute(length1Attr, "length1", "length1", 10.0, kDefaultPreset | kKeyable);
	setMin<double>(length1Attr, 0.01);
	createDoubleAttribute(startToHandleOffsetAttr, "startToHandleOffset", "startToHandleOffset", 10.0, kDefaultPreset | kKeyable);
	setMin<double>(startToHandleOffsetAttr, 0.0);
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
	createAngleAttribute(outputAngle0Attr, "outputAngle0", "outputAngle0", MAngle(60.0, MAngle::kDegrees), kReadOnlyPreset);
	createAngleAttribute(outputAngle1Attr, "outputAngle1", "outputAngle1", MAngle(60.0, MAngle::kDegrees), kReadOnlyPreset);
	createAngleAttribute(outputAngle2Attr, "outputAngle2", "outputAngle2", MAngle(60.0, MAngle::kDegrees), kReadOnlyPreset);
	createDoubleAttribute(outputLength0Attr, "outputLength0", "outputLength0", 10.0, kReadOnlyPreset);
	createDoubleAttribute(outputLength1Attr, "outputLength1", "outputLength1", 10.0, kReadOnlyPreset);
	createDoubleAttribute(outputStartToEffectorOffsetAttr, "outputStartToEffectorOffset", "outputStartToEffectorOffset", 10.0, kReadOnlyPreset);

	addAttribute(length0Attr);
	addAttribute(length1Attr);
	addAttribute(startToHandleOffsetAttr);
	addAttribute(directionAttr);
	addAttribute(solverAttr);
	addAttribute(maxCompressionRatioAttr);
	addAttribute(maxRigidExtensionRatioAttr);
	addAttribute(maxNonRigidExtensionRatioAttr);
	addAttribute(maxHeightReductionRatioAttr);
	addAttribute(heightReductionDecelerationAttr);
	addAttribute(outputAngle0Attr);
	addAttribute(outputAngle1Attr);
	addAttribute(outputAngle2Attr);
	addAttribute(outputLength0Attr);
	addAttribute(outputLength1Attr);
	addAttribute(outputStartToEffectorOffsetAttr);

	attributeAffects(length0Attr, outputAngle0Attr);
	attributeAffects(length1Attr, outputAngle0Attr);
	attributeAffects(startToHandleOffsetAttr, outputAngle0Attr);
	attributeAffects(directionAttr, outputAngle0Attr);
	attributeAffects(solverAttr, outputAngle0Attr);
	attributeAffects(maxCompressionRatioAttr, outputAngle0Attr);
	attributeAffects(maxRigidExtensionRatioAttr, outputAngle0Attr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputAngle0Attr);
	attributeAffects(maxHeightReductionRatioAttr, outputAngle0Attr);
	attributeAffects(heightReductionDecelerationAttr, outputAngle0Attr);

	attributeAffects(length0Attr, outputAngle1Attr);
	attributeAffects(length1Attr, outputAngle1Attr);
	attributeAffects(startToHandleOffsetAttr, outputAngle1Attr);
	attributeAffects(directionAttr, outputAngle1Attr);
	attributeAffects(solverAttr, outputAngle1Attr);
	attributeAffects(maxCompressionRatioAttr, outputAngle1Attr);
	attributeAffects(maxRigidExtensionRatioAttr, outputAngle1Attr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputAngle1Attr);
	attributeAffects(maxHeightReductionRatioAttr, outputAngle1Attr);
	attributeAffects(heightReductionDecelerationAttr, outputAngle1Attr);

	attributeAffects(length0Attr, outputAngle2Attr);
	attributeAffects(length1Attr, outputAngle2Attr);
	attributeAffects(startToHandleOffsetAttr, outputAngle2Attr);
	attributeAffects(directionAttr, outputAngle2Attr);
	attributeAffects(solverAttr, outputAngle2Attr);
	attributeAffects(maxCompressionRatioAttr, outputAngle2Attr);
	attributeAffects(maxRigidExtensionRatioAttr, outputAngle2Attr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputAngle2Attr);
	attributeAffects(maxHeightReductionRatioAttr, outputAngle2Attr);
	attributeAffects(heightReductionDecelerationAttr, outputAngle2Attr);

	attributeAffects(length0Attr, outputLength0Attr);
	attributeAffects(length1Attr, outputLength0Attr);
	attributeAffects(startToHandleOffsetAttr, outputLength0Attr);
	attributeAffects(directionAttr, outputLength0Attr);
	attributeAffects(solverAttr, outputLength0Attr);
	attributeAffects(maxCompressionRatioAttr, outputLength0Attr);
	attributeAffects(maxRigidExtensionRatioAttr, outputLength0Attr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputLength0Attr);
	attributeAffects(maxHeightReductionRatioAttr, outputLength0Attr);
	attributeAffects(heightReductionDecelerationAttr, outputLength0Attr);

	attributeAffects(length0Attr, outputLength1Attr);
	attributeAffects(length1Attr, outputLength1Attr);
	attributeAffects(startToHandleOffsetAttr, outputLength1Attr);
	attributeAffects(directionAttr, outputLength1Attr);
	attributeAffects(solverAttr, outputLength1Attr);
	attributeAffects(maxCompressionRatioAttr, outputLength1Attr);
	attributeAffects(maxRigidExtensionRatioAttr, outputLength1Attr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputLength1Attr);
	attributeAffects(maxHeightReductionRatioAttr, outputLength1Attr);
	attributeAffects(heightReductionDecelerationAttr, outputLength1Attr);

	attributeAffects(length0Attr, outputStartToEffectorOffsetAttr);
	attributeAffects(length1Attr, outputStartToEffectorOffsetAttr);
	attributeAffects(startToHandleOffsetAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(directionAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(solverAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(maxCompressionRatioAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(maxRigidExtensionRatioAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(maxNonRigidExtensionRatioAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(maxHeightReductionRatioAttr, outputStartToEffectorOffsetAttr);
	attributeAffects(heightReductionDecelerationAttr, outputStartToEffectorOffsetAttr);

	return MStatus::kSuccess;
}

MStatus VChainPlanarSolver::setDependentsDirty(const MPlug& plug, MPlugArray& plugArray)
{
	if (plug == length0Attr || plug == length1Attr || plug == startToHandleOffsetAttr || plug == directionAttr || plug == solverAttr ||
		plug == maxCompressionRatioAttr || plug == maxRigidExtensionRatioAttr || plug == maxNonRigidExtensionRatioAttr ||
		plug == maxHeightReductionRatioAttr || plug == heightReductionDecelerationAttr)
		m_evalSinceDirty = false;

	return MStatus::kSuccess;
}

MStatus VChainPlanarSolver::preEvaluation(const MDGContext& context, const MEvaluationNode& evaluationNode)
{
	MStatus status;

	// We only signal evaluation in a normal context
	if (!context.isNormal())
		return MStatus::kFailure;

	if ((evaluationNode.dirtyPlugExists(length0Attr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(length1Attr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(startToHandleOffsetAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(directionAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(solverAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxCompressionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxRigidExtensionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxNonRigidExtensionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(maxHeightReductionRatioAttr, &status) && status) ||
		(evaluationNode.dirtyPlugExists(heightReductionDecelerationAttr, &status) && status))
	{
		m_evalSinceDirty = false;
	}

	return MS::kSuccess;
}

MStatus VChainPlanarSolver::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputAngle0Attr && plug != outputAngle1Attr && plug != outputAngle2Attr && plug != outputLength0Attr &&
		plug != outputLength1Attr && plug != outputStartToEffectorOffsetAttr)
		return MStatus::kUnknownParameter;

	// ------ Evaluation ------
	if (!m_evalSinceDirty)
		computeData(dataBlock);

	// ------ Output ------
	if (plug == outputAngle0Attr)
		outputDoubleValue(dataBlock, outputAngle0Attr, m_solution.B);
	else if (plug == outputAngle1Attr)
		outputDoubleValue(dataBlock, outputAngle1Attr, m_solution.C);
	else if (plug == outputAngle2Attr)
		outputDoubleValue(dataBlock, outputAngle2Attr, m_solution.A);
	else if (plug == outputLength0Attr)
		outputDoubleValue(dataBlock, outputLength0Attr, m_solution.a);
	else if (plug == outputLength1Attr)
		outputDoubleValue(dataBlock, outputLength1Attr, m_solution.b);
	else if (plug == outputStartToEffectorOffsetAttr)
		outputDoubleValue(dataBlock, outputStartToEffectorOffsetAttr, m_solution.c);

	return MStatus::kSuccess;
}

void VChainPlanarSolver::computeData(MDataBlock& dataBlock)
{
	// ------ Triangle ------
	double rigidA = inputDoubleValue(dataBlock, length0Attr);
	double rigidB = inputDoubleValue(dataBlock, length1Attr);
	double handleOffset = inputDoubleValue(dataBlock, startToHandleOffsetAttr);
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

	// ------ Internal State ------
	m_evalSinceDirty = true;
}

// ------ Helpers ------

/*	Solve the angles of a given Triangle whose sides are assumed to be non-zero	lengths */
void VChainPlanarSolver::solveAngles(Triangle& triangle)
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