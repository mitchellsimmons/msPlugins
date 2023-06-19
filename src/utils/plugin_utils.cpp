#include "plugin_utils.h"
#include <sstream>

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

namespace MRS {

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------

// ------ Validation ------
MStatus validateId(const MTypeId& id, std::vector<int> cache)
{
	bool valid = false;
	for (unsigned int i = 0; i < cache.size(); i++) {
		if (cache[i] == id.id())
			valid = true;
	}

	if (valid)
		std::cout << "Validated id : 0x" << std::hex << id.id() << std::endl;
	else
	{
		std::stringstream ss;
		ss <<"MTypeId(" << "0x" << std::hex << id.id() << ")is outside of reserved project id range : {";
		for (int i = 0; i < cache.size(); i++)
		{
			ss << "0x" << std::hex << cache[i];
			if (i != cache.size() - 1)
				ss << ",";
			else
				ss << "}" << std::endl;
		}

		MGlobal::displayError(ss.str().c_str());
	}

	return valid ? MStatus::kSuccess : MStatus::kFailure;
}

// ------------------------------------------------------------------------------------------------------------------------------------------------------------------------

} // MRS

// -------------------------------------------------------------------------------------------------------------------------------------------------------------------------