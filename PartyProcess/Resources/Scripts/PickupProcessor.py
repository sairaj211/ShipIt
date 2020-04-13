import fge
from fge import AreaSearch


def on_init(id):
    pass

def on_event(id,event):
    pass

def on_update(id, dt):
    processor = fge.GetProcessor(id)
    processorContainer = fge.GetCarrier(id)
    processorRecipes = processor.GetRecipes()
    for recipe in processorRecipes:
        recipeMap = recipe[1]
        recipeOutput = recipe[0]
        recipeMatch = True
        for resource in recipeMap:
            recipeMatch = recipeMatch and (len(processorContainer.GetItems(fge.DecodeResource(resource))) == recipeMap[resource])
        if(recipeMatch):
            recipeMatchEvent = fge.GetRecipeMatchEvent()
            recipeMatchEvent.mProcessorID = id
            recipeMatchEvent.mOutput = recipeOutput
            fge.SendEvent(recipeMatchEvent, id)
    if(processor.IsWorking()):
        t = processor.GetCurrentProcessingTime() / processor.GetMaxProcessingTime()
        processorContainer = fge.GetCarrier(id)
        itemIDs = processorContainer.GetItems()
        for itemID in itemIDs:
            itemTransform = fge.GetTransform(itemID)
            itemPosition = itemTransform.GetLocalPosition()
            x = (t - 1.0) * itemPosition[0]
            y = (t - 1.0) * itemPosition[1] + 1.25 * t
            z = (t - 1.0) * itemPosition[2]
            itemTransform.SetLocalPosition(x, y, z)