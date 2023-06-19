import os

from maya import cmds


# ----------------------------------------------------------------------------
# --- CONSTANTS ---
# ----------------------------------------------------------------------------

PACKAGE_NAME = "msPlugins"
MODULE_DIR_PATH = os.path.abspath(cmds.getModulePath(moduleName=PACKAGE_NAME))
PLUGINS_DIR_PATH = os.path.join(MODULE_DIR_PATH, "plugins", "2019")
PLUGIN_DEPENDENCIES_DIR_PATH = os.path.join(PLUGINS_DIR_PATH, "dependencies")


# ----------------------------------------------------------------------------
# --- Plugins ---
# ----------------------------------------------------------------------------

def loadPlugins():
    # Load plugin dependencies
    if os.path.exists(PLUGIN_DEPENDENCIES_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGIN_DEPENDENCIES_DIR_PATH):
            for pluginName in pluginNames:
                if not cmds.pluginInfo(pluginName, q=True, loaded=True):
                    cmds.loadPlugin(pluginName)
            break

    # Load dependent plugins
    if os.path.exists(PLUGINS_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGINS_DIR_PATH):
            for pluginName in pluginNames:
                if not cmds.pluginInfo(pluginName, q=True, loaded=True):
                    cmds.loadPlugin(pluginName)
            break


def unloadPlugins():
    unloadOk = True

    # Check if dependent plugins are ok to unload
    if os.path.exists(PLUGINS_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGINS_DIR_PATH):
            for pluginName in pluginNames:
                if cmds.pluginInfo(pluginName, q=True, loaded=True):
                    if not cmds.pluginInfo(pluginName, q=True, unloadOk=True):
                        unloadOk = False
            break

    # Create a new scene if not
    if not unloadOk:
        cmds.file(force=True, new=True)
        unloadOk = True

    # Unload dependent plugins
    if os.path.exists(PLUGINS_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGINS_DIR_PATH):
            for pluginName in pluginNames:
                if cmds.pluginInfo(pluginName, q=True, loaded=True):
                    cmds.unloadPlugin(pluginName)
            break

    # Check if plugin dependencies are ok to unload
    if os.path.exists(PLUGIN_DEPENDENCIES_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGIN_DEPENDENCIES_DIR_PATH):
            for pluginName in pluginNames:
                if cmds.pluginInfo(pluginName, q=True, loaded=True):
                    if not cmds.pluginInfo(pluginName, q=True, unloadOk=True):
                        unloadOk = False
            break

    # Create a new scene if not
    if not unloadOk:
        cmds.file(force=True, new=True)

    # Unload plugin dependencies
    if os.path.exists(PLUGIN_DEPENDENCIES_DIR_PATH):
        for (dirPath, dirNames, pluginNames) in os.walk(PLUGIN_DEPENDENCIES_DIR_PATH):
            for pluginName in pluginNames:
                if cmds.pluginInfo(pluginName, q=True, loaded=True):
                    cmds.unloadPlugin(pluginName)
            break
