#include "command_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MStatus executeUndoModifierCmd(MDGModifier* dgMod)
{
	MStatus status;

	// Dereference the pointer and store the address of the underlying data as a uintptr_t
	uintptr_t address = (uintptr_t)&*dgMod;

	// Convert address to hex string (ie. pointer addresses are long hex or long long hex values)
	std::ostringstream strAddressStream;
	strAddressStream << std::hex << address;
	std::string strAddress = strAddressStream.str();

	MString cmd = "undoModifier \"";
	cmd += strAddress.c_str();
	cmd += "\"";
	status = MGlobal::executeCommand(cmd, false, true);

	return status;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------