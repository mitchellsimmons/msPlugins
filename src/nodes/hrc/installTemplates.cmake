# Script renames template files and substitutes variable values in the input files with the current user defined prefix before writing contents to output
configure_file("${TEMPLATE_DIR}/NEHrcTemplate.xml.in" "${DIST_INSTALL_DIR}/module/templates/ne/NE${NODE_NAME_PREFIX}HrcTemplate.xml")
configure_file("${TEMPLATE_DIR}/NEModuleHrcTemplate.xml.in" "${DIST_INSTALL_DIR}/module/templates/ne/NE${NODE_NAME_PREFIX}ModuleHrcTemplate.xml")
configure_file("${TEMPLATE_DIR}/NERigHrcTemplate.xml.in" "${DIST_INSTALL_DIR}/module/templates/ne/NE${NODE_NAME_PREFIX}RigHrcTemplate.xml")
