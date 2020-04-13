import fge
from fge import *

CHANGE_TIME = 20.0
TOGGLE_TIME = 0.25
OPEN_ROTATION = [ 0.0, 0.0, 0.0 ]
CLOSED_ROTATION = [ -90.0, 0.0, 0.0 ]

def on_init(id):
    global CHANGE_TIME
    ob = fge.GetObserver(id)
    ob.SetValue("changeTime", CHANGE_TIME)
    ob.SetValue("toggleTime", -1.0)
    transform = fge.GetTransform(id)
    collider = fge.GetCollider(id)
    collider.mIsTrigger = False
    ob.SetValue("isOpen", 1.0)
    if (transform.GetRotation()[0] < 0.0):
        ob.SetValue("isOpen", 0.0)
        collider.mIsTrigger = True

def on_event(id,event):
    pass

def on_update(id, dt):
    global CHANGE_TIME
    global TOGGLE_TIME
    global OPEN_ROTATION
    global CLOSED_ROTATION
    ob = fge.GetObserver(id)
    changeTime = ob.GetValue("changeTime")
    changeTime = changeTime - dt*.001
    isOpen = int(ob.GetValue("isOpen")) == 1
    toggleTime = ob.GetValue("toggleTime")
    if (changeTime < 0.0):
        isOpen = not isOpen
        collider = fge.GetCollider(id)
        if (isOpen):
            collider.mIsTrigger = False
        else:
            collider.mIsTrigger = True
        ob.SetValue("isOpen", isOpen)
        toggleTime = 0.0
        changeTime = CHANGE_TIME
    if (toggleTime >= 0.0):
        toggleTime = toggleTime + (1.0 / TOGGLE_TIME) * dt * .001
        if (toggleTime > 1.0):
            toggleTime = 1.0
        transform = fge.GetTransform(id)
        newRot = []
        if (isOpen):
            newRot = fge.lerp3(CLOSED_ROTATION, OPEN_ROTATION, toggleTime)
        else:
            newRot = fge.lerp3(OPEN_ROTATION, CLOSED_ROTATION, toggleTime)
        transform.SetRotation(newRot[0], newRot[1], newRot[2])
        if (toggleTime >= 1.0):
            toggleTime = -1.0
    ob.SetValue("changeTime", changeTime)
    ob.SetValue("toggleTime", toggleTime)