// Contains basic parsing functions for dealing with names in Maya

#pragma once

#include <algorithm>
#include <iomanip>
#include <string>
#include <sstream>

#include <maya/MString.h>

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

MString getShortName(MString partialName);

MString getPaddedInt(unsigned int n, unsigned int defaultLength = 3);

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------