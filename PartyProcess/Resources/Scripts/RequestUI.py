import fge
from fge import *

def on_init(id):
    pass
    
def on_event(id,event):
    if(event.GetType() == "RequestMadeEvent"):
        materialName = fge.GetMaterial(fge.DecodeGame(event.mGame))
        model = fge.GetModel(id)
        model.mpMaterial = ResourceManager.GetInstance().LoadMaterial(materialName)
        
def on_update(id, dt):
    pass