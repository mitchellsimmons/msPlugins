#pragma once

#include <sstream>
#include <string>

#include <maya/MDGModifier.h>
#include <maya/MGlobal.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MStatus executeUndoModifierCmd(MDGModifier* dgMod);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------