import fge
import struct
from fge import *


windowID = -1

def on_init(id):
    levelbutton = fge.GetLevelButton(id, "LevelButton",True)


def on_event(id,event):
    global windowID
    levelbutton = GetLevelButton(id)
    worldID = levelbutton.WorldID
    if(event.GetType() == "ContactEvent"):
        #print(event.mContactType)
        otherID = -1
        if (event.mId1 == id):
            otherID = event.mId2
        else:
            otherID = event.mId1
        contactType = ContactType
        if(event.mContactType == contactType.TriggerEnter):
            levelbutton.IN = True
        if(event.mContactType == contactType.TriggerExit):
            DeleteEntity(windowID);
            levelbutton.IN = False


       

def on_update(id, dt):
    levelbutton = GetLevelButton(id)
    isUnlocked = levelbutton.isUnlocked
 #   levelID = (levelbutton.LevelID % 5) 
    #levelID = 5 if levelID == 0 else levelID 
    HighScore = levelbutton.HighScore
    LevelName = levelbutton.LevelName
    UnlockScore = levelbutton.UnlockScore
    worldID = levelbutton.WorldID
    Level_str = "Level : {}".format(LevelName)
    score_str = "High Score : {:0.0f}".format(HighScore)
    unlock_str = "Score {:0.0f} on Previous Level".format(UnlockScore)
    
    SceneMan = SceneManager.GetInstance()
    mWidth = SceneMan.ScreenWidth / 2
    mHeight = SceneMan.ScreenHeight / 2 

    if(levelbutton.IN):
       # Renderer.GetInstance().RenderText(Level_str, -(mWidth/2 + 100.0), -(mHeight/1 - 40.0), 1.0, 1, 1, 1, 1)
        Renderer.GetInstance().RenderText(score_str,               -50.0, -(mHeight/1 - 40.0), 1.5, 1, 1, 1, 1)
        if(isUnlocked):
            Renderer.GetInstance().RenderText(" Press Space to Start ", 500.0 , -(mHeight/1 - 40.0), 0.75, 1, 1, 1, 1)
        else:
            Renderer.GetInstance().RenderText(unlock_str , 500.0, -(mHeight/1 - 40.0), 0.75, 1, 1, 1, 1)
