import fge
from fge import *

INITIAL_POS_Z = 0.0
INITIALIZED = False
PAUSED = False

def on_init(id):
    global INITIAL_POS_Z
    global INITIALIZED
    INITIAL_POS_Z = fge.GetTransform(id).GetPosition()[2]

def on_event(id,event):
    global PAUSED
    if(event.GetType() == "PauseEvent"):
        PAUSED = event.mPause

def on_update(id, dt):
    global INITIAL_POS_Z
    global PAUSED
    cam = fge.GetCamera(id)
    im = InputManager.GetInstance()
    if(im.IsTriggered("cam1")):
        cm = CameraManager.GetInstance()
        cm.SetCurrentCamera(cam)
    if (PAUSED):
        return
   