import fge
from fge import *

def on_init(id):
    # Default Resource
    ob = fge.GetObserver(id)
    ob.SetValue("resource", 0)

def on_event(id,event):
    #print(event.GetType())
    if(event.GetType() == "SetCellEvent"):
        resource = event.mResource
        ob = fge.GetObserver(id)
        ob.SetValue("resource", resource)
        cellResource = fge.DecodeResource(resource)
        cellModel = fge.GetModel(id)
        if cellResource == "ProgrammingResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Programming")
        if cellResource == "ArtResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Art")
        if cellResource == "DesignResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Design")
        if cellResource == "PhysicsResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Physics")
        if cellResource == "AudioResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Audio")
        if cellResource == "AIResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("AI")
        if cellResource == "GraphicsResource":
            cellModel.mpMaterial = ResourceManager.GetInstance().LoadMaterial("Graphics")
    if(event.GetType() == "CreateGameEvent"):
        cellContainer = fge.GetCarrier(id)
        items = cellContainer.GetItems()
        for itemID in items:
            cellContainer.RemoveItem(itemID)
            fge.DeleteEntity(itemID)

def on_update(id, dt):
    ob = fge.GetObserver(id)
    resource = fge.DecodeResource(int(ob.GetValue("resource")))
    cellContainer = fge.GetCarrier(id)
    cellItems = fge.GetCarrier(id).GetItems()
    for item in cellItems:
        if resource == fge.GetType(item):
            animator = fge.GetAnimator(item)
            if animator is not None:
                animator.ChangeState("Float")