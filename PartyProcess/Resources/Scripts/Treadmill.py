import fge
from fge import *

FORCE = [ -0.05, 0.0, 0.0 ]

def on_init(id):
    pass

def on_event(id,event):
    global FORCE
    if (event.GetType() == "ContactEvent"):
        otherID = -1
        if (event.mId1 == id):
            otherID = event.mId2
        else:
            otherID = event.mId1
        otherTransform = fge.GetTransform(otherID)
        otherPosition = otherTransform.GetPosition()
        rotatedForce = fge.rotate(FORCE, fge.GetTransform(id))
        otherTransform.SetPosition(otherPosition[0] + rotatedForce[0], otherPosition[1] + rotatedForce[1], otherPosition[2] + rotatedForce[2])

def on_update(id, dt):
    pass