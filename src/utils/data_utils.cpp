/*
	Original work Copyright(c) 2016 Ryan Porter
	Modified work Copyright(c) 2018 Mitchell Simmons
	You may use, distribute, or modify this code under the terms of the MIT license.
*/

#include "data_utils.h"

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Helpers ------
MStatus parseMString(const MString& str, short& outValue)
{
	if (str.isShort())
	{
		outValue = str.asShort();
		return MStatus::kSuccess;
	}
	return MStatus::kFailure;
}

MStatus parseMString(const MString& str, int& outValue)
{
	if (str.isInt())
	{
		outValue = str.asInt();
		return MStatus::kSuccess;
	}
	return MStatus::kFailure;
}

MStatus parseMString(const MString& str, float& outValue)
{
	if (str.isFloat())
	{
		outValue = str.asFloat();
		return MStatus::kSuccess;
	}
	return MStatus::kFailure;
}

MStatus parseMString(const MString& str, double& outValue)
{
	if (str.isDouble())
	{
		outValue = str.asDouble();
		return MStatus::kSuccess;
	}
	return MStatus::kFailure;
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------