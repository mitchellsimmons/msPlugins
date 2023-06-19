#pragma once

#include <stdint.h>
#include <string>

#include <maya/MArgList.h>
#include <maya/MDGModifier.h>
#include <maya/MPxCommand.h>
#include <maya/MString.h>
#include <maya/MSyntax.h>

class UndoModifier : public MPxCommand
{
public:
	UndoModifier();
	~UndoModifier() override;

	// ------ Registration ------
	static const MString kCommandName;
	static MSyntax newSyntax();

	// ------ MPxCommand ------
	bool isUndoable() const override;
	MStatus doIt(const MArgList&) override;
	MStatus redoIt() override;
	MStatus undoIt() override;

private:
	MDGModifier* m_dgMod;
};