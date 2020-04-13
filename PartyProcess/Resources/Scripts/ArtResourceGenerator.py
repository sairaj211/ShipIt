import fge
from fge import *

INPUT_SEQUENCE = [ "action4", "Interact", "Interact" ]

OUTPUT_ITEMS = [ "ArtResource" ]

SEQUENCE_INDEX = 0

INTERACTING = False

def on_init(id):
    pass

def on_event(id,event):
    global INPUT_SEQUENCE
    global OUTPUT_ITEMS
    global SEQUENCE_INDEX
    global INTERACTING
    if(event.GetType() == "GeneratorInputEvent"):
        if(event.mGeneratorID == id):
            requiredInput = INPUT_SEQUENCE[SEQUENCE_INDEX]
            if(event.mInputType == requiredInput):
                audio = fge.GetAudio(id)
                audio.Load("I_A.mp3")
                audio.Play()
                audio.Volume(1.0)
                correctInputEvent = fge.GetInputEvent()
                correctInputEvent.mGeneratorID = id
                fge.SendEvent(correctInputEvent, id)
                SEQUENCE_INDEX = SEQUENCE_INDEX + 1
                if(SEQUENCE_INDEX == len(INPUT_SEQUENCE)):
                    particleEmitter = fge.GetParticles(id)
                    particleEmitter.SetRunning(True)
                    outputEvent = fge.GetGeneratorOutputEvent()
                    outputEvent.mGeneratorID = id
                    outputEvent.mPlayerID = event.mPlayerID
                    outputEvent.mOutputs = OUTPUT_ITEMS
                    fge.SendEvent(outputEvent, id)
                    SEQUENCE_INDEX = 0
    if(event.GetType() == "HighlightEvent"):
        INTERACTING = True

def on_update(id, dt):
    global INPUT_SEQUENCE
    global SEQUENCE_INDEX
    global INTERACTING
    children = fge.GetChildren(id, "Billboard")
    requiredInput = INPUT_SEQUENCE[SEQUENCE_INDEX]
    for childID in children:
        billboardModel = fge.GetModel(childID)
        materialName = billboardModel.mpMaterial.mName
        if (materialName != requiredInput and INTERACTING):
            billboardModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial(requiredInput)
    for childID in children:
        if (fge.IsEnabled(childID) != INTERACTING):
            EntityManager.GetInstance().ToggleEntityEnable(childID, INTERACTING)
    INTERACTING = False