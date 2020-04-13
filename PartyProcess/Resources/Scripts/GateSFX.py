import fge
from fge import *

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "ChangeAnimationStateEvent"):
        if(event.mID == id):
            if(event.mState == "Open"):
                audio = fge.GetAudio(id)
                audio.Load("cartoon_jump-002.wav")
                audio.Play()
                audio.Volume(0.1)
            if(event.mState == "Close"):
                audio = fge.GetAudio(id)
                audio.Load("cartoon_fall-002.wav")
                audio.Play()
                audio.Volume(0.1)

def on_update(id, dt):
    pass