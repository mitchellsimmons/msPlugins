#include "name_utils.h"


// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

/*	Description
	-----------
	Removes the hierarchy and/or namespace from the given partial name    */
MString getShortName(MString partialName)
{
	std::string parseStr = partialName.asChar();
	std::string::size_type n = parseStr.rfind("|");
	if (n == std::string::npos)
		n = parseStr.rfind(":");
	if (n != std::string::npos)
		parseStr = parseStr.substr(n + 1, std::string::npos);

	return parseStr.c_str();
}

/*	Description
	-----------
	Returns a padded string for the given unsigned integer n
	The padding size is equal to the default length minus the number of digits constituting the given integer
	If the default length is less than or equal to the number of digits, the integer will be returned as a string with no additional padding
	
	Example
	-------
	For : n = 13, defaultLength = 3
	Return : 013
*/
MString getPaddedInt(unsigned int n, unsigned int defaultLength)
{
	unsigned int nCopy = n;
	unsigned int numDigits = 0; do { n /= 10; numDigits++; } while (n);
	unsigned int length = std::max(defaultLength, numDigits);
	std::stringstream ss;
	ss << std::setw(length) << std::setfill('0') << nCopy;
	return ss.str().c_str();
}

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------