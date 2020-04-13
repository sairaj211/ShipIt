import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import ContainerOutputEvent
from fge import PlayerPickupEvent

opName = ""
counter = 0

def on_init(id):
    pass

def on_event(id,event):
    global opName
    global counter
    if(event.GetType() == "ContainerOutputEvent"):
        if(event.mContainerID == id):
            processorContainer = fge.GetCarrier(id)
            items = processorContainer.GetItems()
            items.reverse()
            outputItems = items[:event.mNumItems]
            for itemID in outputItems:
                itemCollider = fge.GetCollider(itemID)
                itemCollider.mIsTrigger = False
                itemBody = fge.GetBody(itemID)
                itemBody.mConstrainX = False
                itemBody.mConstrainY = False
                itemBody.mConstrainZ = False
                processorContainer.RemoveItem(itemID)
                pickupEvent = fge.GetPlayerPickupEvent()
                pickupEvent.mPlayerID = event.mPlayerID
                pickupEvent.mItemID = itemID
                fge.SendEvent(pickupEvent, event.mPlayerID)
                item = fge.GetType(itemID)
                if(item == opName and counter == 0):
                    particleEmitter = fge.GetParticles(id)
                    particleEmitter.SetRunning(True)
                    counter += 1
    if(event.GetType() == "RecipeMatchEvent"):
        if(event.mProcessorID == id):
            processor = fge.GetProcessor(id)
            opName = fge.DecodeResource(event.mOutput)
            counter = 0
        
def on_update(id, dt):
    pass