import fge
from fge import *
import math

GRID_ID = -1

def on_init(id):
    pass

def on_event(id,event):
    global GRID_ID
    if(event.GetType() == "SetCellEvent"):
        GRID_ID = event.mGridID
    if(event.GetType() == "ContainerInputEvent"):
        if(event.mContainerID == id):	
            playerHands = fge.GetCarrier(event.mPlayerID)	
            heldItems = playerHands.GetItems()
            inputItems = heldItems[:event.mNumItems]
            dropEvent = fge.GetPlayerDropEvent()
            dropEvent.mPlayerID = event.mPlayerID
            fge.SendEvent(dropEvent, event.mPlayerID)
            cellContainer = fge.GetCarrier(id)
            cellTransform = fge.GetTransform(id)
            cellPosition = cellTransform.GetPosition()
            cellScale = cellTransform.GetScale()
            for itemID in inputItems:
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = True
                itemBody.mConstrainY = True
                itemBody.mConstrainZ = True
                containerLen = len(cellContainer.GetItems())
                cellContainer.AddItem(itemID)
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = True
                itemTransform = fge.GetTransform(itemID)
                itemScale = itemTransform.GetScale()
                itemTransform.SetPosition(cellPosition[0], cellPosition[1] + 0.5 * itemScale[1] + 0.5 * cellScale[1], cellPosition[2])
            ob = fge.GetObserver(id)
            resource = fge.DecodeResource(int(ob.GetValue("resource")))
            if len(inputItems) > 0:
                if resource == fge.GetType(inputItems[0]):
                    itemTransform = fge.GetTransform(inputItems[0])
                    itemScale = itemTransform.GetScale()
                    itemTransform.SetPosition(cellPosition[0], cellPosition[1] + 0.5 * itemScale[1]  + 0.5 * cellScale[1] + 1.0, cellPosition[2])
                    cellUpdateEvent = fge.GetCellUpdateEvent()
                    cellUpdateEvent.mMatch = True
                    cellUpdateEvent.mCellID = id
                    cellUpdateEvent.mPlayerID = event.mPlayerID
                    fge.SendEvent(cellUpdateEvent, GRID_ID)
                    audio = fge.GetAudio(id)
                    audio.Load("GameDeliver01.mp3", False)
                    audio.Play()	
                    audio.Volume(0.1)
                    EntityManager.GetInstance().ToggleEntityEnable(id, False)

def on_update(id, dt):
    pass