import fge
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Collider
from fge import Event
from fge import PlayerPickupEvent


def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "PlayerPickupEvent"):
        if(event.mPlayerID == id):
            audio = fge.GetAudio(id)
            audio.Load("cartoon_jump-002.wav")
            audio.Play()
            audio.Volume(0.1)
            itemID = event.mItemID
            playerHands = fge.GetCarrier(id)
            itemBody = fge.GetBody(itemID)
            if(itemBody is None):
                itemBody = fge.GetRuntimeBody(itemID)
            itemBody.mConstrainX = True
            itemBody.mConstrainY = True
            itemBody.mConstrainZ = True
            playerHands.AddItem(itemID)
            playerScale = fge.GetTransform(id).GetScale()
            itemCollider = fge.GetCollider(itemID)
            if(itemCollider is None):
                itemCollider = fge.GetRuntimeCollider(itemID)
            itemCollider.mIsTrigger = True
            itemTransform = fge.GetTransform(itemID)
            if(itemTransform is None):
                itemTransform = fge.GetRuntimeTransform(itemID)
            itemScale = itemTransform.GetScale()
            itemTransform.SetLocalPosition(0.0, playerScale[1] * 0.5, 0.5 + (itemScale[2] / playerScale[2]) / 2.0)
           # print("Pickup triggered")

def on_update(id, dt):
    pass