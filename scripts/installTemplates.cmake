# Script renames template files and substitutes variable values in the input files with the current user defined prefix before writing contents to output
configure_file("${TEMPLATE_DIR}/msPlugins.mod.in" "${DIST_INSTALL_DIR}/module/msPlugins.mod")
configure_file("${TEMPLATE_DIR}/userSetup.py.in" "${DIST_INSTALL_DIR}/module/scripts/userSetup.py")
configure_file("${TEMPLATE_DIR}/MRS_plugins.py.in" "${DIST_INSTALL_DIR}/module/scripts/python/MRS_plugins.py")