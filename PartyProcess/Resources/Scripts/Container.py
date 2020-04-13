import fge

def on_init(id):
    # Default Game
    ob = fge.GetObserver(id)
    ob.SetValue("game", 0)

def on_event(id,event):
    if(event.GetType() == "RequestMadeEvent"):
        ob = fge.GetObserver(id)
        ob.SetValue("game", event.mGame)

def on_update(id, dt):
    ob = fge.GetObserver(id)
    gameInt = int(ob.GetValue("game"))
    container = fge.GetCarrier(id)
    if(len(container.GetItems("Game")) > 0):
        requestFilledEvent = fge.GetRequestFilledEvent()
        requestFilledEvent.mContainerID = id
        requestFilledEvent.mRequestedGame = gameInt
        fge.BroadcastEvent(requestFilledEvent)
        containerOutputEvent = fge.GetContainerOutputEvent()
        containerOutputEvent.mContainerID = id
        containerOutputEvent.mPlayerID = -1
        containerOutputEvent.mNumItems = 1
        containerOutputEvent.mItemTypes = [ "Game" ]
        fge.SendEvent(containerOutputEvent, id)
        particleEmitter = fge.GetParticles(id)
        particleEmitter.SetRunning(True)
        #containerPosition = fge.GetTransform(id).GetPosition()
        #particleEmitter.SetPosition(containerPosition[0], containerPosition[1], containerPosition[2])
        audio = fge.GetAudio(id)
        audio.Load("ModuleDeliver01.mp3")
        audio.Play()
        audio.Volume(0.2)