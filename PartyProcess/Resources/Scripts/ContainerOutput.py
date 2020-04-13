import fge
from fge import Carrier
from fge import Event
from fge import ContainerOutputEvent

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "ContainerOutputEvent"):
        if(event.mContainerID == id):
            numItems = event.mNumItems
            itemTypes = event.mItemTypes
            container = fge.GetCarrier(id)
            for itemType in itemTypes:
                items = container.GetItems(itemType)
                outputItems = items[:numItems]
                numItems = numItems - len(outputItems)
                for item in items:
                    fge.DeleteEntity(item)
            if(len(itemTypes) == 0):
                items = container.GetItems()
                outputItems = items[:numItems]
                for item in items:
                    fge.DeleteEntity(item)

def on_update(id, dt):
    pass