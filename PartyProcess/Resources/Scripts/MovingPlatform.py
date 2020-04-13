import fge
from fge import *

MOVE_TIME = 5.0
START = [ 0.0, -0.266, -10.0 ]
END = [ 0.0, -0.266, 10.0 ]

def on_init(id):
    global MOVE_TIME
    ob = fge.GetObserver(id)
    ob.SetValue("moveTime", 0.0)
    transform = fge.GetTransform(id)
    ob.SetValue("movingUp", 0.0)
    if (transform.GetPosition()[2] >= 5.0):
        ob.SetValue("movingUp", 1.0)

def on_event(id,event):
    pass

def on_update(id, dt):
    global MOVE_TIME
    global START
    global END
    ob = fge.GetObserver(id)
    movingUp = int(ob.GetValue("movingUp")) == 1
    moveTime = ob.GetValue("moveTime")
    moveTime = moveTime + (1.0 / MOVE_TIME) * dt * .001
    if (moveTime > 1.0):
        moveTime = 1.0
    transform = fge.GetTransform(id)
    newPos = []
    if (movingUp):
        newPos = fge.lerp3(END, START, moveTime)
    else:
        newPos = fge.lerp3(START, END, moveTime)
    transform.SetPosition(newPos[0], newPos[1], newPos[2])
    if (moveTime >= 1.0):
        moveTime = 0.0
    positionZ = transform.GetPosition()[2]
    if (positionZ <= START[2]):
        ob.SetValue("movingUp", 0.0)
    if (positionZ >= END[2]):
        ob.SetValue("movingUp", 1.0)
    ob.SetValue("moveTime", moveTime)