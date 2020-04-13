import fge
from fge import InputEvent

def on_init(id):
    pass

def on_event(id,event):
    if(event.GetType() == "InputEvent"):
        if(event.mGeneratorID == id):
            animator = fge.GetAnimator(id)
            animator.ChangeState("Input")
            #particleEmitter = fge.GetParticles(id)
            #particleEmitter.SetRunning(True)
            #partColor = fge.GetModel(id).mpMaterial.GetDiffuseColor()
            #particleEmitter.r_low = int(partColor[0] * 255.0 * 0.5)
            #particleEmitter.r_high = int(partColor[0] * 255.0)
            #particleEmitter.g_low = int(partColor[1] * 255.0 * 0.5)
            #particleEmitter.g_high = int(partColor[1] * 255.0)
            #particleEmitter.b_low =  int(partColor[2] * 255.0 * 0.5)
            #particleEmitter.b_high = int(partColor[2] * 255.0)
            #generatorPosition = fge.GetTransform(id).GetPosition()
            #particleEmitter.SetPosition(generatorPosition[0], generatorPosition[1], generatorPosition[2])

def on_update(id, dt):
    pass