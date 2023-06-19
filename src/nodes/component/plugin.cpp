#include <maya/MFnPlugin.h>

#include "aim_transform.h"
#include "flexiSpine_locator.h"
#include "flexiSpine_locator_upVector_manip.h"
#include "flexiSpine_locator_scaleAdjustment_manip.h"
#include "flexiSpine_locator_twistAdjustment_manip.h"
#include "flexiSpine_locator_subSceneOverride.h"
#include "flexiChainSingle_locator.h"
#include "flexiChainSingle_locator_upVector_manip.h"
#include "flexiChainSingle_locator_scaleAdjustment_manip.h"
#include "flexiChainSingle_locator_twistAdjustment_manip.h"
#include "flexiChainSingle_locator_subSceneOverride.h"
#include "flexiChainDouble_locator.h"
#include "flexiChainDouble_locator_upVector_manip.h"
#include "flexiChainDouble_locator_scaleAdjustment_manip.h"
#include "flexiChainDouble_locator_twistAdjustment_manip.h"
#include "flexiChainDouble_locator_subSceneOverride.h"
#include "flexiChainTriple_locator.h"
#include "flexiChainTriple_locator_upVector_manip.h"
#include "flexiChainTriple_locator_scaleAdjustment_manip.h"
#include "flexiChainTriple_locator_twistAdjustment_manip.h"
#include "flexiChainTriple_locator_subSceneOverride.h"
#include "flexiInstancer_locator.h"
#include "flexiInstancer_locator_upVector_manip.h"
#include "flexiInstancer_locator_positionAdjustment_manip.h"
#include "flexiInstancer_locator_scaleAdjustment_manip.h"
#include "flexiInstancer_locator_twistAdjustment_manip.h"
#include "flexiInstancer_locator_subSceneOverride.h"
#include "footRoll_node.h"
#include "aim_node.h"
#include "vChainPlanarSolver_node.h"
#include "vChainSolver_node.h"

#include "utils/macros.h"
#include "utils/plugin_utils.h"

// ------ Const ---------------------------------------------------------------------------------------------------------------------------------------------------------------

// Project Block0 IDs : [0x00130fe0 - 0x00130fef]
const MTypeId FlexiSpine::kTypeId = 0x00130fe0;
const MTypeId FlexiSpine_UpVectorManip::kTypeId = 0x00130fe1;
const MTypeId FlexiSpine_UpVectorManipContainer::kTypeId = 0x00130fe2;
const MTypeId FlexiSpine_ScaleAdjustmentManip::kTypeId = 0x00130fe3;
const MTypeId FlexiSpine_ScaleAdjustmentManipContainer::kTypeId = 0x00130fe4;
const MTypeId FlexiSpine_TwistAdjustmentManip::kTypeId = 0x00130fe5;
const MTypeId FlexiSpine_TwistAdjustmentManipContainer::kTypeId = 0x00130fe6;

const MTypeId FlexiChainSingle::kTypeId = 0x00130fe7;
const MTypeId FlexiChainSingle_UpVectorManip::kTypeId = 0x00130fe8;
const MTypeId FlexiChainSingle_UpVectorManipContainer::kTypeId = 0x00130fe9;
const MTypeId FlexiChainSingle_ScaleAdjustmentManip::kTypeId = 0x00130fea;
const MTypeId FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeId = 0x00130feb;
const MTypeId FlexiChainSingle_TwistAdjustmentManip::kTypeId = 0x00130fec;
const MTypeId FlexiChainSingle_TwistAdjustmentManipContainer::kTypeId = 0x00130fed;

const MTypeId FlexiChainDouble::kTypeId = 0x00130fee;
const MTypeId FlexiChainDouble_UpVectorManip::kTypeId = 0x00130fef;
// Project Block1 IDs : [0x00130ff0 - 0x00130fff]
const MTypeId FlexiChainDouble_UpVectorManipContainer::kTypeId = 0x00130ff0;
const MTypeId FlexiChainDouble_ScaleAdjustmentManip::kTypeId = 0x00130ff1;
const MTypeId FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeId = 0x00130ff2;
const MTypeId FlexiChainDouble_TwistAdjustmentManip::kTypeId = 0x00130ff3;
const MTypeId FlexiChainDouble_TwistAdjustmentManipContainer::kTypeId = 0x00130ff4;

const MTypeId FlexiChainTriple::kTypeId = 0x00130ff5;
const MTypeId FlexiChainTriple_UpVectorManip::kTypeId = 0x00130ff6;
const MTypeId FlexiChainTriple_UpVectorManipContainer::kTypeId = 0x00130ff7;
const MTypeId FlexiChainTriple_ScaleAdjustmentManip::kTypeId = 0x00130ff8;
const MTypeId FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeId = 0x00130ff9;
const MTypeId FlexiChainTriple_TwistAdjustmentManip::kTypeId = 0x00130ffa;
const MTypeId FlexiChainTriple_TwistAdjustmentManipContainer::kTypeId = 0x00130ffb;

const MTypeId FlexiInstancer::kTypeId = 0x00130ffc;
const MTypeId FlexiInstancer_UpVectorManip::kTypeId = 0x00130ffd;
const MTypeId FlexiInstancer_UpVectorManipContainer::kTypeId = 0x00130ffe;
// Project Block2 IDs : [0x00131000 - 0x0013100f]
const MTypeId FlexiInstancer_PositionAdjustmentManip::kTypeId = 0x00131000;
const MTypeId FlexiInstancer_PositionAdjustmentManipContainer::kTypeId = 0x00131001;
const MTypeId FlexiInstancer_ScaleAdjustmentManip::kTypeId = 0x00131002;
const MTypeId FlexiInstancer_ScaleAdjustmentManipContainer::kTypeId = 0x00131003;
const MTypeId FlexiInstancer_TwistAdjustmentManip::kTypeId = 0x00131004;
const MTypeId FlexiInstancer_TwistAdjustmentManipContainer::kTypeId = 0x00131005;

const MTypeId AimTransform::kTypeId = 0x00131006;
const MTypeId AimMatrix::kTypeId = 0x00131007;
const MTypeId FootRoll::kTypeId = 0x00131008;
const MTypeId VChainSolver::kTypeId = 0x00131009;
const MTypeId Aim::kTypeId = 0x0013100a;
const MTypeId VChainPlanarSolver::kTypeId = 0x0013100b;

// Names
const MString Aim::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "Aim";
const MString AimTransform::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "AimTransform";
const MString FootRoll::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FootRoll";
const MString VChainSolver::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VChainSolver";
const MString VChainPlanarSolver::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "VChainPlanarSolver";

const MString FlexiSpine::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FlexiSpineShape";
const MString FlexiSpine::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiSpineShape";
const MString FlexiSpine_CounterTwistCommand::kCommandName = "FlexiSpineCounterTwist";
const MString FlexiSpine_StabilityCommand::kCommandName = "FlexiSpineStability";
const MString FlexiSpine_SubSceneOverride::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiSpineShape";
const MString FlexiSpine_SubSceneOverride::kDrawRegistrantId = MRS_XSTR(NODE_NAME_PREFIX) "FlexiSpineShape_SubSceneOverride";
const MString FlexiSpine_UpVectorManip::kTypeName = "FlexiSpineUpVectorManip";
const MString FlexiSpine_UpVectorManipContainer::kTypeName = "FlexiSpineUpVectorManipContainer";
const MString FlexiSpine_UpVectorContextCommand::kCommandName = "FlexiSpineUpVectorContext";
const MString FlexiSpine_ScaleAdjustmentManip::kTypeName = "FlexiSpineScaleAdjustmentManip";
const MString FlexiSpine_ScaleAdjustmentManipContainer::kTypeName = "FlexiSpineScaleAdjustmentManipContainer";
const MString FlexiSpine_ScaleAdjustmentContextCommand::kCommandName = "FlexiSpineScaleAdjustmentContext";
const MString FlexiSpine_TwistAdjustmentManip::kTypeName = "FlexiSpineTwistAdjustmentManip";
const MString FlexiSpine_TwistAdjustmentManipContainer::kTypeName = "FlexiSpineTwistAdjustmentManipContainer";
const MString FlexiSpine_TwistAdjustmentContextCommand::kCommandName = "FlexiSpineTwistAdjustmentContext";

const MString FlexiChainSingle::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainSingleShape";
const MString FlexiChainSingle::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainSingleShape";
const MString FlexiChainSingle_CounterTwistCommand::kCommandName = "FlexiChainSingleCounterTwist";
const MString FlexiChainSingle_StabilityCommand::kCommandName = "FlexiChainSingleStability";
const MString FlexiChainSingle_SubSceneOverride::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainSingleShape";
const MString FlexiChainSingle_SubSceneOverride::kDrawRegistrantId = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainSingleShape_SubSceneOverride";
const MString FlexiChainSingle_UpVectorManip::kTypeName = "FlexiChainSingleUpVectorManip";
const MString FlexiChainSingle_UpVectorManipContainer::kTypeName = "FlexiChainSingleUpVectorManipContainer";
const MString FlexiChainSingle_UpVectorContextCommand::kCommandName = "FlexiChainSingleUpVectorContext";
const MString FlexiChainSingle_ScaleAdjustmentManip::kTypeName = "FlexiChainSingleScaleAdjustmentManip";
const MString FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeName = "FlexiChainSingleScaleAdjustmentManipContainer";
const MString FlexiChainSingle_ScaleAdjustmentContextCommand::kCommandName = "FlexiChainSingleScaleAdjustmentContext";
const MString FlexiChainSingle_TwistAdjustmentManip::kTypeName = "FlexiChainSingleTwistAdjustmentManip";
const MString FlexiChainSingle_TwistAdjustmentManipContainer::kTypeName = "FlexiChainSingleTwistAdjustmentManipContainer";
const MString FlexiChainSingle_TwistAdjustmentContextCommand::kCommandName = "FlexiChainSingleTwistAdjustmentContext";

const MString FlexiChainDouble::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainDoubleShape";
const MString FlexiChainDouble::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainDoubleShape";
const MString FlexiChainDouble_CounterTwistCommand::kCommandName = "FlexiChainDoubleCounterTwist";
const MString FlexiChainDouble_StabilityCommand::kCommandName = "FlexiChainDoubleStability";
const MString FlexiChainDouble_SubSceneOverride::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainDoubleShape";
const MString FlexiChainDouble_SubSceneOverride::kDrawRegistrantId = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainDoubleShape_SubSceneOverride";
const MString FlexiChainDouble_UpVectorManip::kTypeName = "FlexiChainDoubleUpVectorManip";
const MString FlexiChainDouble_UpVectorManipContainer::kTypeName = "FlexiChainDoubleUpVectorManipContainer";
const MString FlexiChainDouble_UpVectorContextCommand::kCommandName = "FlexiChainDoubleUpVectorContext";
const MString FlexiChainDouble_ScaleAdjustmentManip::kTypeName = "FlexiChainDoubleScaleAdjustmentManip";
const MString FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeName = "FlexiChainDoubleScaleAdjustmentManipContainer";
const MString FlexiChainDouble_ScaleAdjustmentContextCommand::kCommandName = "FlexiChainDoubleScaleAdjustmentContext";
const MString FlexiChainDouble_TwistAdjustmentManip::kTypeName = "FlexiChainDoubleTwistAdjustmentManip";
const MString FlexiChainDouble_TwistAdjustmentManipContainer::kTypeName = "FlexiChainDoubleTwistAdjustmentManipContainer";
const MString FlexiChainDouble_TwistAdjustmentContextCommand::kCommandName = "FlexiChainDoubleTwistAdjustmentContext";

const MString FlexiChainTriple::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainTripleShape";
const MString FlexiChainTriple::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainTripleShape";
const MString FlexiChainTriple_CounterTwistCommand::kCommandName = "FlexiChainTripleCounterTwist";
const MString FlexiChainTriple_StabilityCommand::kCommandName = "FlexiChainTripleStability";
const MString FlexiChainTriple_SubSceneOverride::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainTripleShape";
const MString FlexiChainTriple_SubSceneOverride::kDrawRegistrantId = MRS_XSTR(NODE_NAME_PREFIX) "FlexiChainTripleShape_SubSceneOverride";
const MString FlexiChainTriple_UpVectorManip::kTypeName = "FlexiChainTripleUpVectorManip";
const MString FlexiChainTriple_UpVectorManipContainer::kTypeName = "FlexiChainTripleUpVectorManipContainer";
const MString FlexiChainTriple_UpVectorContextCommand::kCommandName = "FlexiChainTripleUpVectorContext";
const MString FlexiChainTriple_ScaleAdjustmentManip::kTypeName = "FlexiChainTripleScaleAdjustmentManip";
const MString FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeName = "FlexiChainTripleScaleAdjustmentManipContainer";
const MString FlexiChainTriple_ScaleAdjustmentContextCommand::kCommandName = "FlexiChainTripleScaleAdjustmentContext";
const MString FlexiChainTriple_TwistAdjustmentManip::kTypeName = "FlexiChainTripleTwistAdjustmentManip";
const MString FlexiChainTriple_TwistAdjustmentManipContainer::kTypeName = "FlexiChainTripleTwistAdjustmentManipContainer";
const MString FlexiChainTriple_TwistAdjustmentContextCommand::kCommandName = "FlexiChainTripleTwistAdjustmentContext";

const MString FlexiInstancer::kTypeName = MRS_XSTR(NODE_NAME_PREFIX) "FlexiInstancerShape";
const MString FlexiInstancer::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiInstancerShape";
const MString FlexiInstancer_CounterTwistCommand::kCommandName = "FlexiInstancerCounterTwist";
const MString FlexiInstancer_StabilityCommand::kCommandName = "FlexiInstancerStability";
const MString FlexiInstancer_SubSceneOverride::kDrawClassification = "drawdb/subscene/" MRS_XSTR(NODE_NAME_PREFIX) "FlexiInstancerShape";
const MString FlexiInstancer_SubSceneOverride::kDrawRegistrantId = MRS_XSTR(NODE_NAME_PREFIX) "FlexiInstancerShape_SubSceneOverride";
const MString FlexiInstancer_UpVectorManip::kTypeName = "FlexiInstancerUpVectorManip";
const MString FlexiInstancer_UpVectorManipContainer::kTypeName = "FlexiInstancerUpVectorManipContainer";
const MString FlexiInstancer_UpVectorContextCommand::kCommandName = "FlexiInstancerUpVectorContext";
const MString FlexiInstancer_PositionAdjustmentManip::kTypeName = "FlexiInstancerPositionAdjustmentManip";
const MString FlexiInstancer_PositionAdjustmentManipContainer::kTypeName = "FlexiInstancerPositionAdjustmentManipContainer";
const MString FlexiInstancer_PositionAdjustmentContextCommand::kCommandName = "FlexiInstancerPositionAdjustmentContext";
const MString FlexiInstancer_ScaleAdjustmentManip::kTypeName = "FlexiInstancerScaleAdjustmentManip";
const MString FlexiInstancer_ScaleAdjustmentManipContainer::kTypeName = "FlexiInstancerScaleAdjustmentManipContainer";
const MString FlexiInstancer_ScaleAdjustmentContextCommand::kCommandName = "FlexiInstancerScaleAdjustmentContext";
const MString FlexiInstancer_TwistAdjustmentManip::kTypeName = "FlexiInstancerTwistAdjustmentManip";
const MString FlexiInstancer_TwistAdjustmentManipContainer::kTypeName = "FlexiInstancerTwistAdjustmentManipContainer";
const MString FlexiInstancer_TwistAdjustmentContextCommand::kCommandName = "FlexiInstancerTwistAdjustmentContext";

// ------ Exports -------------------------------------------------------------------------------------------------------------------------------------------------------------

#define kErrorInvalidPluginId \
	"Plugin \"^1s\" has been given an invalid id"

#define kErrorPluginRegistration \
	"Error registering plugin \"^1s\""

#define kErrorPluginDeregistration \
	"Error deregistering plugin \"^1s\""

#define kErrorFragmentRegistration \
	"Error registering fragment \"^1s\""

#define kErrorFragmentDeregistration \
	"Error deregistering fragment \"^1s\""

MStatus initializePlugin(MObject plugin)
{
	MStatus status;
	MString errorMessage;

	MFnPlugin fnPlugin(plugin, "Mitchell Simmons", MRS_XSTR(PROJECT_VERSION), "Any");
	fnPlugin.setName(MRS_XSTR(TARGET_NAME));

	// Register MEL globals
	MString flexiSpineVar = ("global string $MRS_FlexiSpineType = \"" + FlexiSpine::kTypeName) + "\"";
	MGlobal::executeCommand(flexiSpineVar);
	MString flexiChainSingleVar = ("global string $MRS_FlexiChainSingleType = \"" + FlexiChainSingle::kTypeName) + "\"";
	MGlobal::executeCommand(flexiChainSingleVar);
	MString flexiChainDoubleVar = ("global string $MRS_FlexiChainDoubleType = \"" + FlexiChainDouble::kTypeName) + "\"";
	MGlobal::executeCommand(flexiChainDoubleVar);
	MString flexiChainTripleVar = ("global string $MRS_FlexiChainTripleType = \"" + FlexiChainTriple::kTypeName) + "\"";
	MGlobal::executeCommand(flexiChainTripleVar);
	MString flexiInstancerVar = ("global string $MRS_FlexiInstancerType = \"" + FlexiInstancer::kTypeName) + "\"";
	MGlobal::executeCommand(flexiInstancerVar);

	// |------------|
	// |  Validate  |
	// |------------|

	errorMessage.format(kErrorInvalidPluginId, Aim::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(Aim::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, AimTransform::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AimTransform::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, MString("AimTransform->AimMatrix"));
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(AimMatrix::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FootRoll::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FootRoll::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VChainSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VChainSolver::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, VChainPlanarSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(VChainPlanarSolver::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_UpVectorManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_UpVectorManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_ScaleAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_ScaleAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_TwistAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiSpine_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiSpine_TwistAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_UpVectorManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_UpVectorManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_ScaleAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_TwistAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainSingle_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainSingle_TwistAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_UpVectorManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_UpVectorManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_ScaleAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_TwistAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainDouble_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainDouble_TwistAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_UpVectorManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_UpVectorManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_ScaleAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_TwistAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiChainTriple_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiChainTriple_TwistAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_UpVectorManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_UpVectorManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_ScaleAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_ScaleAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_TwistAdjustmentManip::kTypeId, PROJECT_ID_CACHE), errorMessage);

	errorMessage.format(kErrorInvalidPluginId, FlexiInstancer_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::validateId(FlexiInstancer_TwistAdjustmentManipContainer::kTypeId, PROJECT_ID_CACHE), errorMessage);

	// |------------|
	// |  Register  |
	// |------------|

	errorMessage.format(kErrorPluginRegistration, Aim::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<Aim>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, AimTransform::kTypeName);
	status = MRS::registerTransform<AimMatrix, AimTransform>(fnPlugin);
	MRS_CHECK_ERROR_RETURN_MSTATUS(status, errorMessage);

	errorMessage.format(kErrorPluginRegistration, FootRoll::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<FootRoll>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VChainSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VChainSolver>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, VChainPlanarSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerNode<VChainPlanarSolver>(fnPlugin), errorMessage);

	// FlexiSpine
	errorMessage.format(kErrorPluginRegistration, FlexiSpine_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiSpine_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiSpine_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiSpine_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiSpine_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiSpine_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiSpine_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiSpine_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiSpine_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiSpine_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerSubSceneOverride<FlexiSpine_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiSpine_CounterTwistCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiSpine_StabilityCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiSpine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerLocator<FlexiSpine>(fnPlugin), errorMessage);

	// Create a context for each of our tools
	MGlobal::executeCommandOnIdle("FlexiSpineUpVectorContext FlexiSpineUpVectorContext1");
	MGlobal::executeCommandOnIdle("FlexiSpineScaleAdjustmentContext FlexiSpineScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiSpineTwistAdjustmentContext FlexiSpineTwistAdjustmentContext1");

	// Setup the marking menu callback
	MGlobal::executeCommandOnIdle("callbacks -addCallback MRS_FlexiSpine_rmbCallback -hook addRMBBakingMenuItems -owner FlexiSpine;");

	// FlexiChainSingle
	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainSingle_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainSingle_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainSingle_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainSingle_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainSingle_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainSingle_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainSingle_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainSingle_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainSingle_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerSubSceneOverride<FlexiChainSingle_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainSingle_CounterTwistCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainSingle_StabilityCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainSingle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerLocator<FlexiChainSingle>(fnPlugin), errorMessage);

	// Create a context for each of our tools
	MGlobal::executeCommandOnIdle("FlexiChainSingleUpVectorContext FlexiChainSingleUpVectorContext1");
	MGlobal::executeCommandOnIdle("FlexiChainSingleScaleAdjustmentContext FlexiChainSingleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiChainSingleTwistAdjustmentContext FlexiChainSingleTwistAdjustmentContext1");

	// Setup the marking menu callback
	MGlobal::executeCommandOnIdle("callbacks -addCallback MRS_FlexiChainSingle_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainSingle;");

	// FlexiChainDouble
	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainDouble_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainDouble_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainDouble_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainDouble_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainDouble_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainDouble_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainDouble_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainDouble_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainDouble_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerSubSceneOverride<FlexiChainDouble_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainDouble_CounterTwistCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainDouble_StabilityCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainDouble::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerLocator<FlexiChainDouble>(fnPlugin), errorMessage);

	// Create a context for each of our tools
	MGlobal::executeCommandOnIdle("FlexiChainDoubleUpVectorContext FlexiChainDoubleUpVectorContext1");
	MGlobal::executeCommandOnIdle("FlexiChainDoubleScaleAdjustmentContext FlexiChainDoubleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiChainDoubleTwistAdjustmentContext FlexiChainDoubleTwistAdjustmentContext1");

	// Setup the marking menu callback
	MGlobal::executeCommandOnIdle("callbacks -addCallback MRS_FlexiChainDouble_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainDouble;");

	// FlexiChainTriple
	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainTriple_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainTriple_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainTriple_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainTriple_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainTriple_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainTriple_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiChainTriple_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiChainTriple_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiChainTriple_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerSubSceneOverride<FlexiChainTriple_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainTriple_CounterTwistCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiChainTriple_StabilityCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiChainTriple::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerLocator<FlexiChainTriple>(fnPlugin), errorMessage);

	// Create a context for each of our tools
	MGlobal::executeCommandOnIdle("FlexiChainTripleUpVectorContext FlexiChainTripleUpVectorContext1");
	MGlobal::executeCommandOnIdle("FlexiChainTripleScaleAdjustmentContext FlexiChainTripleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiChainTripleTwistAdjustmentContext FlexiChainTripleTwistAdjustmentContext1");

	// Setup the marking menu callback
	MGlobal::executeCommandOnIdle("callbacks -addCallback MRS_FlexiChainTriple_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainTriple;");

	// FlexiInstancer
	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiInstancer_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiInstancer_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiInstancer_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_PositionAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiInstancer_PositionAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_PositionAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiInstancer_PositionAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_PositionAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiInstancer_PositionAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiInstancer_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiInstancer_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiInstancer_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipulator<FlexiInstancer_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerManipContainer<FlexiInstancer_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerContextCommand<FlexiInstancer_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerSubSceneOverride<FlexiInstancer_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiInstancer_CounterTwistCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerCommand<FlexiInstancer_StabilityCommand>(fnPlugin, true /* syntax */), errorMessage);

	errorMessage.format(kErrorPluginRegistration, FlexiInstancer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::registerLocator<FlexiInstancer>(fnPlugin), errorMessage);

	// Create a context for each of our tools
	MGlobal::executeCommandOnIdle("FlexiInstancerUpVectorContext FlexiInstancerUpVectorContext1");
	MGlobal::executeCommandOnIdle("FlexiInstancerPositionAdjustmentContext FlexiInstancerPositionAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiInstancerScaleAdjustmentContext FlexiInstancerScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("FlexiInstancerTwistAdjustmentContext FlexiInstancerTwistAdjustmentContext1");

	// Setup the marking menu callback
	MGlobal::executeCommandOnIdle("callbacks -addCallback MRS_FlexiInstancer_rmbCallback -hook addRMBBakingMenuItems -owner FlexiInstancer;");

	// Shared setup
	// Find XML and register our 2D texture sampler fragment
	errorMessage.format(kErrorFragmentRegistration, MString("mrsTexture2D"));
	MString texture2DFragmentFile = MGlobal::executeCommandStringResult("MRS_findTexture2D()", false, false, &status);
	if (!status || !texture2DFragmentFile.length())
	{
		std::cerr << errorMessage << std::endl;
		return MStatus::kFailure;
	}

	MRenderer* renderer = MRenderer::theRenderer();
	if (!renderer)
	{
		std::cerr << errorMessage << std::endl;
		return MStatus::kFailure;
	}

	MFragmentManager* fragmentManager = renderer->getFragmentManager();
	if (!fragmentManager)
	{
		std::cerr << errorMessage << std::endl;
		return MStatus::kFailure;
	}

	MString fragmentAdded;
	if (!fragmentManager->hasFragment("mrsTexture2D"))
	{
		fragmentAdded = fragmentManager->addShadeFragmentFromFile(texture2DFragmentFile, false);

		if (!fragmentAdded.length())
		{
			std::cerr << errorMessage << std::endl;
			return MStatus::kFailure;
		}
	}

	return MStatus::kSuccess;
}

MStatus uninitializePlugin(MObject plugin)
{
	MString errorMessage;

	MFnPlugin fnPlugin(plugin);

	// |--------------|
	// |  Deregister  |
	// |--------------|

	errorMessage.format(kErrorPluginDeregistration, Aim::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<Aim>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, AimTransform::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<AimTransform>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FootRoll::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FootRoll>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VChainSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VChainSolver>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, VChainPlanarSolver::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<VChainPlanarSolver>(fnPlugin), errorMessage);

	// FlexiSpine
	MGlobal::executeCommand("callbacks -removeCallback MRS_FlexiSpine_rmbCallback -hook addRMBBakingMenuItems -owner FlexiSpine;");

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiSpine_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiSpine_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiSpine_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterSubSceneOverride<FlexiSpine_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiSpine_CounterTwistCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiSpine_StabilityCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiSpine::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiSpine>(fnPlugin), errorMessage);
	
	MGlobal::executeCommandOnIdle("deleteUI FlexiSpineUpVectorContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiSpineScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiSpineTwistAdjustmentContext1");

	// FlexiChainSingle
	MGlobal::executeCommand("callbacks -removeCallback MRS_FlexiChainSingle_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainSingle;");

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainSingle_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainSingle_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainSingle_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterSubSceneOverride<FlexiChainSingle_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainSingle_CounterTwistCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainSingle_StabilityCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainSingle::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainSingle>(fnPlugin), errorMessage);

	MGlobal::executeCommandOnIdle("deleteUI FlexiChainSingleUpVectorContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainSingleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainSingleTwistAdjustmentContext1");

	// FlexiChainDouble
	MGlobal::executeCommand("callbacks -removeCallback MRS_FlexiChainDouble_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainDouble;");

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainDouble_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainDouble_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainDouble_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterSubSceneOverride<FlexiChainDouble_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainDouble_CounterTwistCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainDouble_StabilityCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainDouble::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainDouble>(fnPlugin), errorMessage);

	MGlobal::executeCommandOnIdle("deleteUI FlexiChainDoubleUpVectorContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainDoubleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainDoubleTwistAdjustmentContext1");

	// FlexiChainTriple
	MGlobal::executeCommand("callbacks -removeCallback MRS_FlexiChainTriple_rmbCallback -hook addRMBBakingMenuItems -owner FlexiChainTriple;");

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainTriple_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainTriple_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiChainTriple_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterSubSceneOverride<FlexiChainTriple_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainTriple_CounterTwistCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiChainTriple_StabilityCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiChainTriple::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiChainTriple>(fnPlugin), errorMessage);

	MGlobal::executeCommandOnIdle("deleteUI FlexiChainTripleUpVectorContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainTripleScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiChainTripleTwistAdjustmentContext1");

	// FlexiInstancer
	MGlobal::executeCommand("callbacks -removeCallback MRS_FlexiInstancer_rmbCallback -hook addRMBBakingMenuItems -owner FlexiInstancer;");

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_UpVectorContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiInstancer_UpVectorContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_UpVectorManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_UpVectorManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_UpVectorManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_UpVectorManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_PositionAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiInstancer_PositionAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_PositionAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_PositionAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_PositionAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_PositionAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_ScaleAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiInstancer_ScaleAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_ScaleAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_ScaleAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_ScaleAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_ScaleAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_TwistAdjustmentContextCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterContextCommand<FlexiInstancer_TwistAdjustmentContextCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_TwistAdjustmentManipContainer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_TwistAdjustmentManipContainer>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_TwistAdjustmentManip::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer_TwistAdjustmentManip>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_SubSceneOverride::kDrawRegistrantId);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterSubSceneOverride<FlexiInstancer_SubSceneOverride>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_CounterTwistCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiInstancer_CounterTwistCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer_StabilityCommand::kCommandName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterCommand<FlexiInstancer_StabilityCommand>(fnPlugin), errorMessage);

	errorMessage.format(kErrorPluginDeregistration, FlexiInstancer::kTypeName);
	MRS_CHECK_ERROR_RETURN_MSTATUS(MRS::deregisterNode<FlexiInstancer>(fnPlugin), errorMessage);

	MGlobal::executeCommandOnIdle("deleteUI FlexiInstancerUpVectorContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiInstancerPositionAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiInstancerScaleAdjustmentContext1");
	MGlobal::executeCommandOnIdle("deleteUI FlexiInstancerTwistAdjustmentContext1");

	// Shared cleanup
	errorMessage.format(kErrorFragmentDeregistration, MString("mrsTexture2D"));
	MRenderer* renderer = MRenderer::theRenderer();
	if (!renderer)
	{
		std::cerr << errorMessage << std::endl;
		return MStatus::kFailure;
	}

	MFragmentManager* fragmentManager = renderer->getFragmentManager();
	if (!fragmentManager)
	{
		std::cerr << errorMessage << std::endl;
		return MStatus::kFailure;
	}

	if (fragmentManager->hasFragment("mrsTexture2D"))
	{
		if (!fragmentManager->removeFragment("mrsTexture2D"))
		{
			std::cerr << errorMessage << std::endl;
			return MStatus::kFailure;
		}
	}
	
	return MStatus::kSuccess;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------