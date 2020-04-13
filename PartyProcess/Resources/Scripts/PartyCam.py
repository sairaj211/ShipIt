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
    cameraTransform = fge.GetTransform(id)
    cameraPos = cameraTransform.GetPosition()
    sumX = 0.0
    sumZ = 6.0 * INITIAL_POS_Z
    playerIDs = fge.GetEntities("Player")
    numPlayers = len(playerIDs)
    for playerID in playerIDs:
        playerTransform = fge.GetTransform(playerID)
        sumX += playerTransform.GetPosition()[0]
        sumZ += playerTransform.GetPosition()[2] + INITIAL_POS_Z
    sumX /= len(playerIDs) + 6
    sumZ /= len(playerIDs) + 6
    cameraPos[0] = cameraPos[0] + dt * (sumX - cameraPos[0]) * 0.001
    cameraPos[2] = cameraPos[2] + dt * (sumZ - cameraPos[2]) * 0.001
    cameraTransform.SetPosition(cameraPos[0], cameraPos[1], cameraPos[2])