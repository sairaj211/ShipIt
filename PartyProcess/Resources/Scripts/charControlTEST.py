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
        particles.SetPosition(playerPos[0], playerPos[1], playerPos[2])
        particles.SetRunning(True)
        sprint = con.IsTriggered("Sprint")
        if(sprint):
            x = 2*x
            y = 2*y
        player.Move(x,y,sprint)
    body  =  fge.GetBody(id)
    vel = body.GetVelocity()
    xV = vel[0]
    zV = vel[2]