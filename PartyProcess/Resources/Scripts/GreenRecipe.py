import fge
from fge import Carrier
from fge import RecipeMatchEvent

RECIPE = {
    "YellowResource": 3
}

OUTPUT = {
	"GreenResource": 1
}

def on_init(id):
    pass

def on_event(id,event):
    pass

def on_update(id, dt):
    global RECIPE
    global OUTPUT
    processorContainer = fge.GetCarrier(id)
    recipeMatch = True
    for resource in RECIPE:
        recipeMatch = recipeMatch and (len(processorContainer.GetItems(resource)) == RECIPE[resource])
    if(recipeMatch):
        recipeMatchEvent = fge.GetRecipeMatchEvent()
        recipeMatchEvent.mProcessorID = id
        tmpOutput = []
        for resource in OUTPUT:
            tmpOutput.append((resource, OUTPUT[resource]))
        recipeMatchEvent.mOutputTypeQuantity = tmpOutput
        fge.SendEvent(recipeMatchEvent, id)