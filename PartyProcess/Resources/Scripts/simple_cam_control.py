import fge
from fge import *

def on_init(id):
    pass

def on_event(id,event):
    pass

def on_update(id, dt):
    con = fge.GetController(id)
    rot_sp = 1
    mv_sp = 20
    cam = fge.GetCamera(id)
    move_x = move_y = move_z = rot_x = rot_y = 0
    if(con.IsPressed("action14")):
        if(con.IsPressed("r_up") ):
            rot_y += rot_sp
        elif(con.IsPressed("r_down")) :
            rot_y -= rot_sp
        if(con.IsPressed("r_left")):
             move_z += mv_sp
        elif(con.IsPressed("r_right")):
            move_z -= mv_sp
    else:
        if(con.IsPressed("r_up")):
            move_x += mv_sp
        elif(con.IsPressed("r_down")):
            move_x -= mv_sp
        if(con.IsPressed("r_right")):
            move_y += mv_sp
        elif(con.IsPressed("r_left")):
            move_y -= mv_sp
        if(con.IsPressed("action10") ):
            rot_x += rot_sp
        elif(con.IsPressed("action9")):
            rot_x -= rot_sp
    
    mouse = Mouse.GetInstance()
    if(mouse.IsPressed(1)):
        m_x = mouse.MouseDeltaX()
        m_y = mouse.MouseDeltaY()
        if(m_x < 0 ):
            rot_x += rot_sp * .5
        elif(m_x > 0):
            rot_x -= rot_sp  * .5
        if(m_y < 0):
            rot_y += rot_sp  * .5
        elif(m_y > 0):
            rot_y -= rot_sp  * .5
    if(mouse.ScrollUp()):
        move_z += mv_sp
    elif(mouse.ScrollDown()):
        move_z -= mv_sp

    cam.RotateCamera(rot_x,rot_y,True)
    cam.MoveCamera(dt,move_y,move_z,move_x)
    
    im = InputManager.GetInstance()
    if(im.IsTriggered("cam1")):
        cm = CameraManager.GetInstance()
        cm.SetCurrentCamera(cam)

   