import fge
from fge import *
import math

ITEM_SPACING = 0.5

def on_init(id):
    pass

def on_event(id,event):
    global ITEM_SPACING
    if(event.GetType() == "ContainerInputEvent"):
        if(event.mContainerID == id):
            #audio = fge.GetAudio(id)
            #audio.Load("GameDeliver01.mp3", False)
            #audio.Play()	
            #audio.Volume(0.1)	
            playerHands = fge.GetCarrier(event.mPlayerID)	
            heldItems = playerHands.GetItems()
            inputItems = heldItems[:event.mNumItems]
            dropEvent = fge.GetPlayerDropEvent()
            dropEvent.mPlayerID = event.mPlayerID
            fge.SendEvent(dropEvent, event.mPlayerID)
            blockContainer = fge.GetCarrier(id)
            for itemID in inputItems:
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = True
                itemBody.mConstrainY = True
                itemBody.mConstrainZ = True
                blockContainer.AddItem(itemID)
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = True
                itemTransform = fge.GetTransform(itemID)
                itemScale = itemTransform.GetLocalScale()
                itemTransform.SetLocalPosition(0.0, 0.5 + 0.5 * itemScale[1], 0.0)

def on_update(id, dt):
    pass