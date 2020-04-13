import fge
from fge import Carrier
from fge import ResourceManager

REQUEST_UI_ID = -1
INIT = True
RECIPE = [ "BlueResource", "PurpleResource", "PurpleResource", "BlueResource", "YellowResource", "BlueResource", "GreenResource", "GreenResource", "GreenResource" ]
OUTPUT = "Platformer"

def on_init(id):
    global REQUEST_UI_ID
    global INIT
    global RECIPE
    global OUTPUT
    if(INIT):
        REQUEST_UI_ID = fge.CreateEntity("UI")
        uiTransform = fge.GetRuntimeTransform(REQUEST_UI_ID)
        uiTransform.SetScale(128.0, 128.0, 0.0)
        uiModel = fge.GetRuntimeModel(REQUEST_UI_ID)
        uiModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("PlatformerRecipe")
        requestMadeEvent = fge.GetRequestMadeEvent()
        requestMadeEvent.mContainerID = id
        requestMadeEvent.mRequestUIID = REQUEST_UI_ID
        requestMadeEvent.mGameRecipe = RECIPE
        requestMadeEvent.mOutput = OUTPUT
        fge.SendEvent(requestMadeEvent, id)
    INIT = False

def on_event(id,event):
    pass

def on_update(id, dt):
    container = fge.GetCarrier(id)
    if(len(container.GetItems("Platformer")) > 0):
        requestFilledEvent = fge.GetRequestFilledEvent()
        requestFilledEvent.mContainerID = id
        requestFilledEvent.mRequestUIID = REQUEST_UI_ID
        fge.BroadcastEvent(requestFilledEvent)
        containerOutputEvent = fge.GetContainerOutputEvent()
        containerOutputEvent.mContainerID = id
        containerOutputEvent.mPlayerID = -1
        containerOutputEvent.mNumItems = 1
        containerOutputEvent.mItemTypes = [ "Platformer" ]
        fge.SendEvent(containerOutputEvent, id)