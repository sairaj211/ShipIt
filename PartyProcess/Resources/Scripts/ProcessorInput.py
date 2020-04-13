import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import ContainerInputEvent
import math

ITEM_SPACING = 0.5

def on_init(id):
    pass

def on_event(id,event):
    global ITEM_SPACING
    if(event.GetType() == "ContainerInputEvent"):
        if(event.mContainerID == id):
            audio = fge.GetAudio(id)
            audio.Load("GameDeliver01.mp3", False)
            audio.Play()	
            audio.Volume(0.1)	
            playerHands = fge.GetCarrier(event.mPlayerID)	
            heldItems = playerHands.GetItems()
            inputItems = heldItems[:event.mNumItems]
            dropEvent = fge.GetPlayerDropEvent()
            dropEvent.mPlayerID = event.mPlayerID
            fge.SendEvent(dropEvent, event.mPlayerID)
            processorContainer = fge.GetCarrier(id)
            for itemID in inputItems:
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = True
                itemBody.mConstrainY = True
                itemBody.mConstrainZ = True
                containerLen = len(processorContainer.GetItems())
                processorContainer.AddItem(itemID)
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = True
                itemTransform = fge.GetTransform(itemID)
                if (containerLen % 2) == 0:
                    itemTransform.SetLocalPosition(0, 1.25, math.ceil(containerLen / 2.0) * ITEM_SPACING)
                else:
                    itemTransform.SetLocalPosition(0, 1.25, -1 * math.ceil(containerLen / 2.0) * ITEM_SPACING)

def on_update(id, dt):
    pass