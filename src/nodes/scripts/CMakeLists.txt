project(node_scripts VERSION 1.0.0)

set(TEMPLATE_FILES	
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEnewPrimitiveArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEnewVectorArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEnewMatrixArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEreplacePrimitiveArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEreplaceVectorArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEreplaceMatrixArray.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEspacer.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_AEseparator.mel"
	"${CMAKE_CURRENT_SOURCE_DIR}/templates/MRS_rampTemplate.mel")

# Install AE templates
install(FILES ${TEMPLATE_FILES}
	DESTINATION "${DIST_INSTALL_DIR}/module/templates/ae")