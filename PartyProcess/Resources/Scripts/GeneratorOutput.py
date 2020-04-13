import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import GeneratorOutputEvent
import math

LAUNCH_SPEED = 4

def on_init(id):
    pass

def on_event(id,event):
    global LAUNCH_SPEED
    if(event.GetType() == "GeneratorOutputEvent"):
        if(event.mGeneratorID == id):
            outputItems = event.mOutputs
            playerPosition = fge.GetTransform(event.mPlayerID).GetPosition()
            generatorTransform = fge.GetTransform(id)
            generatorPosition = generatorTransform.GetPosition()
            generatorScale = generatorTransform.GetScale()
            for i in range(0, len(outputItems)):
                outputItem = fge.CreateEntity(outputItems[i])
                itemBody = fge.GetRuntimeBody(outputItem)
                itemBody.Init()
                if(i == 0):
                    pickupEvent = fge.GetPlayerPickupEvent()
                    pickupEvent.mPlayerID = event.mPlayerID
                    pickupEvent.mItemID = outputItem
                    fge.SendEvent(pickupEvent, event.mPlayerID)
                else:
                    itemTransform = fge.GetRuntimeTransform(outputItem)
                    itemScale = itemTransform.GetScale()
                    itemTransform.SetPosition(generatorPosition[0], generatorPosition[1] + 0.5 * (generatorScale[1] + itemScale[1]), generatorPosition[2])
                    toPlayer = [ playerPosition[0] - generatorPosition[0], playerPosition[1] - generatorPosition[1], playerPosition[2] - generatorPosition[2] ]
                    itemBody.SetVelocity(LAUNCH_SPEED * toPlayer[0], LAUNCH_SPEED * toPlayer[1], LAUNCH_SPEED * toPlayer[2])
                    print("reached YEET")

def on_update(id, dt):
    pass