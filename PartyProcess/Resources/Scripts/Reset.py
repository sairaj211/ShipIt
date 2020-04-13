import fge
from fge import *

def on_init(id):
    ob = fge.GetObserver(id)
    transform = fge.GetTransform(id)
    position = transform.GetPosition()
    ob.SetValue("resetX", position[0])
    ob.SetValue("resetY", position[1])
    ob.SetValue("resetZ", position[2])

def on_event(id,event):
    pass

def on_update(id, dt):
    transform = fge.GetTransform(id)
    position = transform.GetPosition()
    if (position[1] < -10.0):
        carrier = fge.GetCarrier(id)
        if (carrier is not None):
            items = carrier.GetItems()
            carrier.Void()
            for item in items:
                if (fge.GetType(item) == "Game"):
                    resetGridEvent = fge.GetResetGridEvent()
                    fge.BroadcastEvent(resetGridEvent)
                fge.DeleteEntity(item)
        ob = fge.GetObserver(id)
        transform.SetPosition(ob.GetValue("resetX"), ob.GetValue("resetY"), ob.GetValue("resetZ"))
        transform.SetRotation(0.0, 0.0, 0.0)
        player = fge.GetPlayer(id)
        player.Rotate(0.0)
        body = fge.GetBody(id)
        if (body is not None):
            body.SetVelocity(0.0, 0.0, 0.0)
       