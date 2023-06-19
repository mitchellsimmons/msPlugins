"""
Automation script, creates build system templates from each of our Maya template files
The build system then uses the template to apply a user-defined prefix to each of the Maya templates
"""

import os
import re
import shutil

DIRECTORY = "C:\\Users\\mitch\\Documents\\maya\\2019\\prefs\\viewTemplates"


def main():
    # The output string is to be used by the build system
    output = ""

    for filename in os.listdir(DIRECTORY):
        if filename.startswith("NEmrs_") and not filename.endswith(".bak"):
            src = os.path.join(DIRECTORY, filename)
            dst = os.path.join(DIRECTORY, re.sub("mrs_", "", filename) + ".in")
            shutil.copy(src, dst)

            with open(dst, "w") as outFile:
                with open(src, "r") as inFile:
                    for line in inFile:
                        line = re.sub("mrs_", "${NODE_NAME_PREFIX}", line)
                        outFile.write(line)

            os.rename(src, src + ".bak")
            output += "configure_file(\"${PROJECT_DIR}/scripts/templates/" + re.sub("mrs_", "", filename) + ".in\" \"${DIST_INSTALL_DIR}/module/templates/ne/NE${NODE_NAME_PREFIX}" + re.sub("NEmrs_", "", filename) + "\")\n"

    print output


if __name__ == '__main__':
    main()
