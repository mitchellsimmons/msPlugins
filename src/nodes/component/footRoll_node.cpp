#include "footRoll_node.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	Foot-roll can be driven backwards (pivoting from the heel) or forwards (pivoting from the toes then the tip)
	Backwards foot-roll is entirely defined by the heelBackDriver attribute which is responsible for defining the driver range for heel-back rotation
	Forwards foot-roll is defined by three driver stages:
	1.	The tarsusLockDriver attribute defines the driver range over which the tarsus will reach the tarsusLimit angle
	2.	The tarsusLockedDriver attribute defines the driver range over which the tarsus remains locked at the tarsusLimit angle and the toe begins to roll
	3.	The tarsusStraightenDriver attribute defines the driver range over which the tarsus straightens (the amount of straightening is given by the tarsusStraightenFactor)
		The toe continues to roll over this driver range, up until the toeLimit value is reached
	There are additional smoothing attributes which define the interpolation between stages (a value of 0.0 sets interpolation to linear)
	The output values are designed to be applied to their respective rest values
*/

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

FootRoll::FootRoll() {}
FootRoll::~FootRoll() {}

// ------ Attr ------
MObject FootRoll::rollAttr;
MObject FootRoll::heelBackDriverAttr;
MObject FootRoll::tarsusLockDriverAttr;
MObject FootRoll::tarsusLockedDriverAttr;
MObject FootRoll::tarsusStraightenDriverAttr;
MObject FootRoll::tarsusLimitAttr;
MObject FootRoll::toeLimitAttr;
MObject FootRoll::tarsusStraightenFactorAttr;
MObject FootRoll::heelSmoothFactorAttr;
MObject FootRoll::tarsusSmoothFactorAttr;
MObject FootRoll::toeSmoothFactorAttr;
MObject FootRoll::outputTarsusAttr;
MObject FootRoll::outputToeAttr;
MObject FootRoll::outputHeelAttr;

// ------ MPxNode ------
MPxNode::SchedulingType FootRoll::schedulingType() const
{
	return SchedulingType::kParallel;
}

MStatus FootRoll::initialize()
{
	createAngleAttribute(rollAttr, "roll", "roll", 0.0, kDefaultPreset | kKeyable);
	setMinMax(rollAttr, MAngle(-M_PI), MAngle(M_PI));
	createAngleAttribute(heelBackDriverAttr, "heelBackDriver", "heelBackDriver", MAngle(-100.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(heelBackDriverAttr, MAngle(-M_PI), MAngle(0.0));
	createAngleAttribute(tarsusLockDriverAttr, "tarsusLockDriver", "tarsusLockDriver", MAngle(40.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(tarsusLockDriverAttr, MAngle(0.0), MAngle(M_PI));
	createAngleAttribute(tarsusLockedDriverAttr, "tarsusLockedDriver", "tarsusLockedDriver", MAngle(40.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(tarsusLockedDriverAttr, MAngle(0.0), MAngle(M_PI));
	createAngleAttribute(tarsusStraightenDriverAttr, "tarsusStraightenDriver", "tarsusStraightenDriver", MAngle(20.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(tarsusStraightenDriverAttr, MAngle(0.0), MAngle(M_PI));
	createAngleAttribute(tarsusLimitAttr, "tarsusLimit", "tarsusLimit", MAngle(20.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(tarsusLimitAttr, MAngle(0.0), MAngle(M_PI));
	createAngleAttribute(toeLimitAttr, "toeLimit", "toeLimit", MAngle(90.0, MAngle::kDegrees), kDefaultPreset | kKeyable);
	setMinMax(toeLimitAttr, MAngle(0.0), MAngle(M_PI));
	createDoubleAttribute(tarsusStraightenFactorAttr, "tarsusStraightenFactor", "tarsusStraightenFactor", 1.0, kDefaultPreset | kKeyable);
	setMinMax(tarsusStraightenFactorAttr, 0.0, 1.0);
	createDoubleAttribute(heelSmoothFactorAttr, "heelSmoothFactor", "heelSmoothFactor", 0.5, kDefaultPreset | kKeyable);
	setMinMax(heelSmoothFactorAttr, 0.0, 1.0);
	createDoubleAttribute(tarsusSmoothFactorAttr, "tarsusSmoothFactor", "tarsusSmoothFactor", 0.5, kDefaultPreset | kKeyable);
	setMinMax(tarsusSmoothFactorAttr, 0.0, 1.0);
	createDoubleAttribute(toeSmoothFactorAttr, "toeSmoothFactor", "toeSmoothFactor", 0.5, kDefaultPreset | kKeyable);
	setMinMax(toeSmoothFactorAttr, 0.0, 1.0);
	createAngleAttribute(outputHeelAttr, "outputHeel", "outputHeel", 0.0, kReadOnlyPreset);
	createAngleAttribute(outputTarsusAttr, "outputTarsus", "outputTarsus", 0.0, kReadOnlyPreset);
	createAngleAttribute(outputToeAttr, "outputToe", "outputToe", 0.0, kReadOnlyPreset);

	addAttribute(rollAttr);
	addAttribute(heelBackDriverAttr);
	addAttribute(tarsusLockDriverAttr);
	addAttribute(tarsusLockedDriverAttr);
	addAttribute(tarsusStraightenDriverAttr);
	addAttribute(tarsusLimitAttr);
	addAttribute(toeLimitAttr);
	addAttribute(tarsusStraightenFactorAttr);
	addAttribute(heelSmoothFactorAttr);
	addAttribute(tarsusSmoothFactorAttr);
	addAttribute(toeSmoothFactorAttr);
	addAttribute(outputHeelAttr);
	addAttribute(outputTarsusAttr);
	addAttribute(outputToeAttr);

	attributeAffects(rollAttr, outputHeelAttr);
	attributeAffects(heelBackDriverAttr, outputHeelAttr);
	attributeAffects(heelSmoothFactorAttr, outputHeelAttr);

	attributeAffects(rollAttr, outputTarsusAttr);
	attributeAffects(tarsusLockDriverAttr, outputTarsusAttr);
	attributeAffects(tarsusLockedDriverAttr, outputTarsusAttr);
	attributeAffects(tarsusStraightenDriverAttr, outputTarsusAttr);
	attributeAffects(tarsusLimitAttr, outputTarsusAttr);
	attributeAffects(tarsusStraightenFactorAttr, outputTarsusAttr);
	attributeAffects(tarsusSmoothFactorAttr, outputTarsusAttr);

	attributeAffects(rollAttr, outputToeAttr);
	attributeAffects(tarsusLockDriverAttr, outputToeAttr);
	attributeAffects(tarsusLockedDriverAttr, outputToeAttr);
	attributeAffects(tarsusStraightenDriverAttr, outputToeAttr);
	attributeAffects(toeLimitAttr, outputToeAttr);
	attributeAffects(toeSmoothFactorAttr, outputToeAttr);

	return MStatus::kSuccess;
}

MStatus FootRoll::compute(const MPlug& plug, MDataBlock& dataBlock)
{
	if (plug != outputHeelAttr && plug != outputTarsusAttr && plug != outputToeAttr)
		return MStatus::kUnknownParameter;

	// Driver inputs
	double roll = inputAngleValue(dataBlock, rollAttr).asRadians();
	double heelBackDriver = inputAngleValue(dataBlock, heelBackDriverAttr).asRadians();
	double tarsusLockDriver = inputAngleValue(dataBlock, tarsusLockDriverAttr).asRadians();
	double tarsusLockedDriver = inputAngleValue(dataBlock, tarsusLockedDriverAttr).asRadians();
	double tarsusStraightenDriver = inputAngleValue(dataBlock, tarsusStraightenDriverAttr).asRadians();

	// Forward-roll stages
	double stage1LateBound = tarsusLockDriver;
	double stage2LateBound = stage1LateBound + tarsusLockedDriver;
	double stage3LateBound = stage2LateBound + tarsusStraightenDriver;

	if (plug == outputHeelAttr)
	{
		double heelSmoothFactor = inputDoubleValue(dataBlock, heelSmoothFactorAttr);

		if (roll < 0.0)
		{
			roll = std::max(roll, heelBackDriver);
			double t = 1.0 - MRS::remap(roll, heelBackDriver, 0.0, 0.0, 1.0);
			outputAngleValue(dataBlock, outputHeelAttr, MRS::variableSmoothstep(0.0, heelBackDriver, t, heelSmoothFactor));
		}
		else
			outputAngleValue(dataBlock, outputHeelAttr, 0.0);
	}
	else if (plug == outputTarsusAttr)
	{
		double tarsusLimit = inputAngleValue(dataBlock, tarsusLimitAttr).asRadians();
		double straightenFactor = inputDoubleValue(dataBlock, tarsusStraightenFactorAttr);
		double tarsusSmoothFactor = inputDoubleValue(dataBlock, tarsusSmoothFactorAttr);

		if (roll > 0.0)
		{
			// Stage 1
			if (roll <= stage1LateBound)
			{
				double t = MRS::remap(roll, 0.0, stage1LateBound, 0.0, 1.0);
				outputAngleValue(dataBlock, outputTarsusAttr, MRS::variableSmoothstep(0.0, tarsusLimit, t, tarsusSmoothFactor));
			}
			// Stage 2
			else if (roll <= stage2LateBound)
			{
				outputAngleValue(dataBlock, outputTarsusAttr, tarsusLimit);
			}
			// Stage 3
			else
			{
				roll = std::min(roll, stage3LateBound);
				double t = MRS::remap(roll, stage2LateBound, stage3LateBound, 0.0, 1.0);
				double maxStraighten = tarsusLimit * straightenFactor;
				double lateBound = tarsusLimit - maxStraighten;
				outputAngleValue(dataBlock, outputTarsusAttr, MRS::variableSmoothstep(tarsusLimit, lateBound, t, tarsusSmoothFactor));
			}
		}
		else
			outputAngleValue(dataBlock, outputTarsusAttr, 0.0);
	}
	else if (plug == outputToeAttr)
	{
		double toeLimit = inputAngleValue(dataBlock, toeLimitAttr).asRadians();
		double toeSmoothFactor = inputDoubleValue(dataBlock, toeSmoothFactorAttr);

		if (roll > stage1LateBound)
		{
			roll = std::min(roll, stage3LateBound);
			double t = MRS::remap(roll, stage1LateBound, stage3LateBound, 0.0, 1.0);
			outputAngleValue(dataBlock, outputToeAttr, MRS::variableSmoothstep(0.0, toeLimit, t, toeSmoothFactor));
		}
		else
			outputAngleValue(dataBlock, outputToeAttr, 0.0);
	}

	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------