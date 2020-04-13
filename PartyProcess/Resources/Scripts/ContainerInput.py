import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import ContainerInputEvent

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "ContainerInputEvent"):
        if(event.mContainerID == id):
            playerHands = fge.GetCarrier(event.mPlayerID)		
            heldItems = playerHands.GetItems()
            inputItems = heldItems[:event.mNumItems]
            dropEvent = fge.GetPlayerDropEvent()
            dropEvent.mPlayerID = event.mPlayerID
            fge.SendEvent(dropEvent, event.mPlayerID)
            container = fge.GetCarrier(id)
            for itemID in inputItems:
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = True
                itemBody.mConstrainY = True
                itemBody.mConstrainZ = True
                container.AddItem(itemID)
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = True
                itemTransform = fge.GetTransform(itemID)
                itemTransform.SetLocalPosition(0, -4, 0)

def on_update(id, dt):
    pass