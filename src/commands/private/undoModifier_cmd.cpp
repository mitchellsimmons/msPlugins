/*	Credit
	------
	To Serguei Kalentchouk for original idea and Python implementation
	Source: https://medium.com/@k_serguei/maya-python-api-2-0-and-the-undo-stack-80b84de70551    */

/*	Description
	-----------
	A generic command which can be used to execute MDGModifer function calls with the addition of having them injected onto Maya's undo queue
	This is less general than the Python version in that the Python version will accept any object with a command-like interface (ie. implements redoIt() and undoIt() functions)
	Whereas the C++ compiler requires that it knows the type of all member variables before runtime    */

#include "undoModifier_cmd.h"

UndoModifier::UndoModifier() 
	: m_dgMod{ nullptr }
{}

UndoModifier::~UndoModifier() 
{
	if (m_dgMod)
		delete m_dgMod;
}

// ------ Registration ------

MSyntax UndoModifier::newSyntax()
{
	MSyntax syntax;

	syntax.addArg(MSyntax::kString);

	syntax.enableQuery(false);
	syntax.enableEdit(false);

	return syntax;
}

// ------ MPxCommand ------

bool UndoModifier::isUndoable() const
{
	return true;
}

MStatus UndoModifier::doIt(const MArgList& args)
{
	MStatus status;

	// Retrieve hex string
	MString strAddress = args.asString(0);

	// Convert hex string back to uintptr_t address
	uintptr_t address;
	if (sizeof(uintptr_t) == sizeof(unsigned long))
		// 32 bit system
		address = std::strtoul(strAddress.asChar(), nullptr, 16);
	else
		// 64 bit system
		address = std::strtoull(strAddress.asChar(), nullptr, 16);
	
	// Retrieve pointer to original address
	m_dgMod = reinterpret_cast<MDGModifier*>(address);

	// Execute
	return redoIt();
}

MStatus UndoModifier::redoIt()
{
	if (m_dgMod)
		return m_dgMod->doIt();

	return MStatus::kFailure;
}

MStatus UndoModifier::undoIt()
{
	if (m_dgMod)
		return m_dgMod->undoIt();

	return MStatus::kFailure;
}