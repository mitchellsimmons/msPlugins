#include <maya/MGlobal.h>
#include <maya/MStatus.h>
#include <maya/MString.h>

// The argument will not be macro-expanded by the preprocessor
#define MRS_STR(a) #a
// The argument will be macro-expanded afterwhich its contents will be stringized
#define MRS_XSTR(a) MRS_STR(a)

// Check status and return it if invalid
#define MRS_CHECK_ERROR_RETURN_MSTATUS(status, msg) \
{ \
	MStatus maya_status = status; \
	if (!maya_status) \
	{ \
		MString maya_error_string = __FILE__; \
		maya_error_string += " : line "; \
		maya_error_string += __LINE__; \
		maya_error_string += " : "; \
		maya_error_string += maya_status.errorString(); \
		maya_error_string += " : "; \
		maya_error_string += msg; \
		MGlobal::displayError(maya_error_string); \
		return status; \
	} \
}