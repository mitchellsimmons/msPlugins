#pragma once

#include <vector>

#include <maya/MAngle.h>
#include <maya/MString.h>
#include <maya/MTypeId.h>

#include "utils/data_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// Custom DG data object used to read and write angle-array data
// As with single element angular attributes, the user is free to use a standard double-array type as an alternative
// This type simply exists to provide angular type specificity in-line with Maya's existing conventions
class AngleArrayData : public MRS::DataArrayHelper<MAngle, double, 1>
{
public:
	AngleArrayData();
	~AngleArrayData();

	// ------ Const ------
	static const MTypeId kTypeId;
	static const MString kTypeName;

	// ------ MPxData ------
	MTypeId typeId() const override;
	MString name() const override;

	// ------ MRS::DataArrayHelper ------
	unsigned int length() const override;
	void getArray(std::vector<MAngle>& outArray) const override;
	void setArray(const std::vector<MAngle>& array) override;
};

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------