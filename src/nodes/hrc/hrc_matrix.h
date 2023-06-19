#pragma once

#include <maya/MMatrix.h>
#include <maya/MPxTransformationMatrix.h>
#include <maya/MTypeId.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

class HrcMatrix : public MPxTransformationMatrix
{
public:
	HrcMatrix();
	~HrcMatrix();

	// ------ Const ------
	const static MTypeId kTypeId;

	// ------ MPxTransformationMatrix ------
	MMatrix asMatrix() const override;
	MMatrix asMatrix(double percent) const override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------
