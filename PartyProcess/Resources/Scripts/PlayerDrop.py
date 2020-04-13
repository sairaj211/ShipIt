import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import PlayerDropEvent

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "PlayerDropEvent"):
        if(event.mPlayerID == id):
            playerHands = fge.GetCarrier(id)
            heldItems = playerHands.GetItems()
            for itemID in heldItems:
                audio = fge.GetAudio(id)
                audio.Load("cartoon_fall-002.wav")
                audio.Play()
                audio.Volume(0.1)
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = False
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = False
                itemBody.mConstrainY = False
                itemBody.mConstrainZ = False
            playerHands.Void()

def on_update(id, dt):
    pass