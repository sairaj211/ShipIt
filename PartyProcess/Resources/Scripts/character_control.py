import fge
from fge import Player
from fge import Controller
from fge import Collider
from fge import Animator
from fge import Body
from fge import AreaSearch
from fge import Carrier
from fge import Transform
from fge import PlayerPickupEvent
from fge import PlayerDropEvent
from fge import Event
from fge import Processor
from fge import ContainerInputEvent
from fge import ContainerOutputEvent
from fge import ContainerMatchEvent

def on_init(id):
    player = fge.GetPlayer(id, "Player", True)
    player.SetMaxVelocity(10.0)

def on_event(id,event):
    pass

def on_update(id, dt):
    con = fge.GetController(id)
    player = fge.GetPlayer(id)
    #update the player
    speed = 8.0
    x = y = 0
    rot_num = 0
    rot = 0
    particles = fge.GetParticles(id)
    if(con.IsPressed("up")):
        rot += 180
        rot_num +=1
        y -= speed
    elif(con.IsPressed("down")):
        rot += 0
        rot_num +=1
        y += speed
    if(con.IsPressed("left")):
        if(con.IsPressed("up")):
            rot += 270
        else:
            rot += -90
        rot_num +=1
        x -= speed
    elif(con.IsPressed("right")):
        rot += 90
        rot_num +=1
        x += speed
    if(rot_num >= 1):
        player.Rotate(rot/rot_num)
    if(x!=0 or y != 0):
        playerTransform = fge.GetTransform(id)
        playerPos = playerTransform.GetPosition()
        playerScale = playerTransform.GetScale()
        #particles.SetPosition(playerPos[0], playerPos[1], playerPos[2])
        particles.SetRunning(True)
        sprint = con.IsTriggered("Sprint")
        if(sprint):
            x = 2*x
            y = 2*y
        player.Move(x,y,sprint)
    else:
        particles.SetRunning(False)
    playerHands = fge.GetCarrier(id)		
    heldItems = playerHands.GetItems()
    lenHeldItems = len(heldItems)
    pickupAreaIDs = fge.GetChildren(id, "SearchArea")
    for pickupAreaID in pickupAreaIDs:
        pickupArea = fge.GetAreaSearch(pickupAreaID)
        foundIDs = pickupArea.GetFound()
        for foundID in foundIDs:
            foundType = fge.GetType(foundID)
            if(lenHeldItems == 0):
                if(foundType == "Processor"):
                     container = fge.GetCarrier(foundID)
                     processor = fge.GetProcessor(foundID)
                     if(len(container.GetItems()) > 0 and not processor.IsWorking()):
                         if(con.IsTriggered("Interact")):
                             containerOutputEvent = fge.GetContainerOutputEvent()
                             containerOutputEvent.mPlayerID = id
                             containerOutputEvent.mContainerID = foundID
                             containerOutputEvent.mNumItems = 1
                             fge.SendEvent(containerOutputEvent, foundID)
                         else:
                             containerModel = fge.GetModel(foundID)
                             if containerModel is not None:
                                 containerModel.Highlight()
                         break
                if(foundType == "Block"):
                     container = fge.GetCarrier(foundID)
                     if(len(container.GetItems()) > 0):
                         if(con.IsTriggered("Interact")):
                             containerOutputEvent = fge.GetContainerOutputEvent()
                             containerOutputEvent.mPlayerID = id
                             containerOutputEvent.mContainerID = foundID
                             containerOutputEvent.mNumItems = 1
                             fge.SendEvent(containerOutputEvent, foundID)
                         else:
                             containerModel = fge.GetModel(foundID)
                             if containerModel is not None:
                                 containerModel.Highlight()
                         break
                elif(foundType == "Container"):
                    continue
                elif(foundType == "Generator"):
                    inputType = ""
                    if(con.IsTriggered("Interact")):
                        inputType = "Interact"
                    if(con.IsTriggered("Sprint")):
                        inputType = "Sprint"
                    if(con.IsTriggered("action3")):
                        inputType = "action3"
                    if(con.IsTriggered("action4")):
                        inputType = "action4"
                    if(inputType != ""):
                        generatorInputEvent = fge.GetGeneratorInputEvent()
                        generatorInputEvent.mGeneratorID = foundID
                        generatorInputEvent.mPlayerID = id
                        generatorInputEvent.mInputType = inputType
                        fge.SendEvent(generatorInputEvent, foundID)
                    else:
                        highlightEvent = fge.GetHighlightEvent()
                        fge.SendEvent(highlightEvent, foundID)
                        itemModel = fge.GetModel(foundID)
                        if itemModel is not None:
                            itemModel.Highlight()
                    break
                elif(foundType == "GridCell"):
                     container = fge.GetCarrier(foundID)
                     if(len(container.GetItems()) > 0):
                         if(con.IsTriggered("Interact")):
                             containerOutputEvent = fge.GetContainerOutputEvent()
                             containerOutputEvent.mPlayerID = id
                             containerOutputEvent.mContainerID = foundID
                             containerOutputEvent.mNumItems = 1
                             fge.SendEvent(containerOutputEvent, foundID)
                         else:
                             containerModel = fge.GetModel(foundID)
                             if containerModel is not None:
                                 containerModel.Highlight()
                         break
                elif(not fge.HasParent(foundID) or fge.GetType(fge.GetParentID(foundID)) == "Platform"):
                    if(con.IsTriggered("Interact")):
                        pickupEvent = fge.GetPlayerPickupEvent()
                        pickupEvent.mPlayerID = id
                        pickupEvent.mItemID = foundID
                        fge.SendEvent(pickupEvent, id)
                    else:
                        itemModel = fge.GetModel(foundID)
                        if itemModel is not None:
                            itemModel.Highlight()
                    break
            else:
                if(fge.GetType(heldItems[0]) != "Game"):
                    if(foundType == "Processor"):
                        processor = fge.GetProcessor(foundID)
                        processorContainter = fge.GetCarrier(foundID)
                        if(processor.GetMaxInput() > len(processorContainter.GetItems())):
                            if(con.IsTriggered("Interact")):
                                containerInputEvent = fge.GetContainerInputEvent()
                                containerInputEvent.mPlayerID = id
                                containerInputEvent.mContainerID = foundID
                                containerInputEvent.mNumItems = 1
                                fge.SendEvent(containerInputEvent, foundID)
                            else:
                                processorModel = fge.GetModel(foundID)
                                if processorModel is not None:
                                    processorModel.Highlight()
                            break
                    if(foundType == "Block"):
                        container = fge.GetCarrier(foundID)
                        block = fge.GetBlock(foundID)
                        if(1 > len(container.GetItems()) and block.CanInput()):
                            if(con.IsTriggered("Interact")):
                                containerInputEvent = fge.GetContainerInputEvent()
                                containerInputEvent.mPlayerID = id
                                containerInputEvent.mContainerID = foundID
                                containerInputEvent.mNumItems = 1
                                fge.SendEvent(containerInputEvent, foundID)
                            else:
                                processorModel = fge.GetModel(foundID)
                                if processorModel is not None:
                                    processorModel.Highlight()
                        break
                    if(foundType == "GridCell"):
                        cellContainter = fge.GetCarrier(foundID)
                        if(1 > len(cellContainter.GetItems())):
                            if(con.IsTriggered("Interact")):
                                containerInputEvent = fge.GetContainerInputEvent()
                                containerInputEvent.mPlayerID = id
                                containerInputEvent.mContainerID = foundID
                                containerInputEvent.mNumItems = 1
                                fge.SendEvent(containerInputEvent, foundID)
                            else:
                                cellModel = fge.GetModel(foundID)
                                if cellModel is not None:
                                    cellModel.Highlight()
                            break
                else:
                    if(foundType == "Container"):
                        if(con.IsTriggered("Interact")):
                            containerInputEvent = fge.GetContainerInputEvent()
                            containerInputEvent.mPlayerID = id
                            containerInputEvent.mContainerID = foundID
                            containerInputEvent.mNumItems = 1
                            fge.SendEvent(containerInputEvent, foundID)
                        else:
                            processorModel = fge.GetModel(foundID)
                            if processorModel is not None:
                                processorModel.Highlight()
                        break
        if(len(foundIDs) == 0 and lenHeldItems > 0 and fge.GetType(heldItems[0]) != "Game" and con.IsTriggered("Interact")):
            dropEvent = fge.GetPlayerDropEvent()
            dropEvent.mPlayerID = id
            fge.SendEvent(dropEvent, id)
    body  =  fge.GetBody(id)
    vel = body.GetVelocity()
    xV = vel[0]
    zV = vel[2]