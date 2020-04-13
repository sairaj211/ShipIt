import fge
from fge import *

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "ContainerOutputEvent"):
        if(event.mContainerID == id):
            blockContainer = fge.GetCarrier(id)
            items = blockContainer.GetItems()
            items.reverse()
            outputItems = items[:event.mNumItems]
            for itemID in outputItems:
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = False
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = False
                itemBody.mConstrainY = False
                itemBody.mConstrainZ = False
                blockContainer.RemoveItem(itemID)
                pickupEvent = fge.GetPlayerPickupEvent()
                pickupEvent.mPlayerID = event.mPlayerID
                pickupEvent.mItemID = itemID
                fge.SendEvent(pickupEvent, event.mPlayerID)

def on_update(id, dt):
    pass