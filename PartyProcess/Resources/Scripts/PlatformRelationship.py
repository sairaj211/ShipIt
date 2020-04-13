import fge
from fge import *

def on_init(id):
    pass

def on_event(id,event):
    if (event.GetType() == "ContactEvent"):
        otherType = ""
        otherID = -1
        if (event.mId1 == id):
            if (EntityManager.GetInstance().GetEntity(event.mId2, False) is not None):
                otherType = fge.GetType(event.mId2)
                otherID = event.mId2
        else:
            if (EntityManager.GetInstance().GetEntity(event.mId1, False) is not None):
                otherType = fge.GetType(event.mId1)
                otherID = event.mId1
        #player = fge.GetPlayer(id)
        if (otherType == "Platform"):
            if (event.mContactType == ContactType.CollisionEnter):
                body = fge.GetBody(id)
                body.mConstrainY = True
                ob = fge.GetObserver(id)
                ob.SetValue("oldParent", fge.GetParentID(id))
                fge.SetParentID(id, otherID);
            if (event.mContactType == ContactType.CollisionExit):
                ob = fge.GetObserver(id)
                oldParent = int(ob.GetValue("oldParent"))
                if (fge.GetParentID(id) is otherID):
                    body = fge.GetBody(id)
                    body.mConstrainY = False
                    fge.SetParentID(id, oldParent);
                

def on_update(id, dt):
    pass