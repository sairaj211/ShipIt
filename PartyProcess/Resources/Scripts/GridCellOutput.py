import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import ContainerOutputEvent
from fge import PlayerPickupEvent

GRID_ID = -1

def on_init(id):
    pass

def on_event(id,event):
    global GRID_ID
    if(event.GetType() == "SetCellEvent"):
        GRID_ID = event.mGridID
    if(event.GetType() == "ContainerOutputEvent"):
        if(event.mContainerID == id):
            cellContainer = fge.GetCarrier(id)
            items = cellContainer.GetItems()
            items.reverse()
            outputItems = items[:event.mNumItems]
            for itemID in outputItems:
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = False
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = False
                itemBody.mConstrainY = False
                itemBody.mConstrainZ = False
                animator = fge.GetAnimator(itemID)
                if animator is not None:
                    animator.ChangeState("None")
                cellContainer.RemoveItem(itemID)
                pickupEvent = fge.GetPlayerPickupEvent()
                pickupEvent.mPlayerID = event.mPlayerID
                pickupEvent.mItemID = itemID
                fge.SendEvent(pickupEvent, event.mPlayerID)
            if len(cellContainer.GetItems()) == 0:
                cellUpdateEvent = fge.GetCellUpdateEvent()
                cellUpdateEvent.mMatch = False
                cellUpdateEvent.mCellID = id
                cellUpdateEvent.mPlayerID = event.mPlayerID
                fge.SendEvent(cellUpdateEvent, GRID_ID)

def on_update(id, dt):
    pass