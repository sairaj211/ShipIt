import fge
from fge import Collider
from fge import Body
from fge import Carrier
from fge import Transform
from fge import Event
from fge import RecipeMatchEvent
import math

ITEM_SPACING = 0.5

def on_init(id):
    pass

def on_event(id,event):
    global ITEM_SPACING
    if(event.GetType() == "RecipeMatchEvent"):
        if(event.mProcessorID == id):
            processor = fge.GetProcessor(id)
            if(processor.IsDone()):
                processorContainer = fge.GetCarrier(id)
                items = processorContainer.GetItems()
                processorContainer.Void()
                for item in items:
                    fge.DeleteEntity(item)
                outputItem = fge.CreateEntity(fge.DecodeResource(event.mOutput))
                #print(fge.DecodeResource(event.mOutput))
                itemBody = fge.GetRuntimeBody(outputItem)
                itemBody.Init()
                itemBody.mConstrainX = True
                itemBody.mConstrainY = True
                itemBody.mConstrainZ = True
                processorContainer.AddItem(outputItem)
                itemCollider = fge.GetRuntimeCollider(outputItem)
                itemCollider.mIsTrigger = True
                itemTransform = fge.GetRuntimeTransform(outputItem)
                itemTransform.SetLocalPosition(0, 1.25, 0)
                itemPosition = itemTransform.GetPosition()
                itemColor = fge.GetRuntimeModel(outputItem).mpMaterial.GetDiffuseColor()
                #particleEmitter = fge.GetParticles(id)
                #particleEmitter.SetRunning(True)
                #particleEmitter.r_low = int(itemColor[0] * 255.0 * 0.5)
                #particleEmitter.r_high = int(itemColor[0] * 255.0)
                #particleEmitter.g_low = int(itemColor[1] * 255.0 * 0.5)
                #particleEmitter.g_high = int(itemColor[1] * 255.0)
                #particleEmitter.b_low =  int(itemColor[2] * 255.0 * 0.5)
                #particleEmitter.b_high = int(itemColor[2] * 255.0)
                #particleEmitter.SetPosition(itemPosition[0], itemPosition[1], itemPosition[2])
                processor.Stop()
                audio = fge.GetAudio(id)
                audio.Stop()
            elif(not processor.IsWorking()):
                processor.Start()
                audio = fge.GetAudio(id)
                audio.Load("MergingMaster02.mp3", True)
                audio.Play()
                audio.Volume(0.1)
                
def on_update(id, dt):
    pass