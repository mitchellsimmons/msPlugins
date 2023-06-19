import maya.cmds as cmds
import maya.OpenMayaUI as omui
from PySide2 import QtWidgets, QtCore
import shiboken2
#from maya.app.general.mayaMixin import MayaQWidgetDockableMixin

# ------ Scale Adjustment Dock ---------------------------------------------------------------------------------------------------------------------------------------

if not '_scaleAdjustmentDockWidget' in globals():
    _scaleAdjustmentDockWidget = None

_scaleAdjustmentDockName = "MRS_FlexiChainTriple_ScaleAdjustmentUI_WorkspaceControl"
_scaleAdjustmentWindowName = "MRS_FlexiChainTriple_ScaleAjustmentUI"

def scaleAdjustmentDockUIScript():
    """ This function is called when creating the workspace control for the first time
        It is also called if the workspace control already exists upon starting a new session """

    # We must check if the contents of the workspace control exist
    # If they do not exist, then this function was called as a result of Maya starting up
    # In this case we want to hide the control as it will not be showing anything
    if not cmds.window(_scaleAdjustmentWindowName, q = True, exists = True):
        cmds.workspaceControl(_scaleAdjustmentDockName, e=True, visible=False)


# Could also use MayaQWidgetDockableMixin, however this seems to just complicate things
# Refer to : <drive>\Program Files\Autodesk\Maya<version>\Python\Lib\site-packages\maya\app\general\mayaMixin.py
def scaleAdjustmentDockUI():
    global _scaleAdjustmentDockWidget

    if _scaleAdjustmentDockWidget is None:
        # If the workspace control exists it may be remnant from a previous session and does not need to be created
        if not cmds.workspaceControl(_scaleAdjustmentDockName, q = True, exists = True):
            # The uiScript is a required flag, we will encounter strange issues such as the workspace control disappearing when the workspace is changed (eg. on ctrl+space) if not set
            # - It is better to set it to "pass" than to not set it at all
            # - It is called upon starting a new session, if the workspace control was not closed before ending the last session
            # - It is also called upon creating the workspace control for the first time
            uiScript = "import MRS_FlexiChainTriple_scaleAdjustmentUIDock\nMRS_FlexiChainTriple_scaleAdjustmentUIDock.scaleAdjustmentDockUIScript()"
            cmds.workspaceControl(_scaleAdjustmentDockName, heightProperty = "preferred", initialHeight = 330, initialWidth = 1100, uiScript = uiScript)

    dockPtr = omui.MQtUtil.findControl(_scaleAdjustmentDockName)
    _scaleAdjustmentDockWidget = shiboken2.wrapInstance(long(dockPtr), QtWidgets.QWidget)
    _scaleAdjustmentDockWidget.setMinimumHeight(330)
    # If we set the children of a control using Qt we must ensure setObjectName is used otherwise Maya commands will not recognise the paths to those children
    _scaleAdjustmentDockWidget.setObjectName(_scaleAdjustmentDockName)
    cmds.workspaceControl(_scaleAdjustmentDockName, edit = True, label = cmds.window(_scaleAdjustmentWindowName, q = True, title = True))

    # Clear current children
    for i in reversed(range(_scaleAdjustmentDockWidget.layout().count())):
        child = _scaleAdjustmentDockWidget.layout().itemAt(i).widget()
        if child:
            child.setParent(None)
            child.deleteLater()

    # Retrieve and attach the main widget
    windowPtr = omui.MQtUtil.findWindow(_scaleAdjustmentWindowName)
    window = shiboken2.wrapInstance(long(windowPtr), QtWidgets.QWidget)
    window.setObjectName(_scaleAdjustmentWindowName)
    _scaleAdjustmentDockWidget.layout().addWidget(window)

    # Ensure the control is maximized if docked or visible if hidden
    if cmds.workspaceControl(_scaleAdjustmentDockName, q=True, visible=True):
        cmds.workspaceControl(_scaleAdjustmentDockName, e=True, restore=True)
    else:
        cmds.workspaceControl(_scaleAdjustmentDockName, e=True, visible=True)
        
    return _scaleAdjustmentDockWidget