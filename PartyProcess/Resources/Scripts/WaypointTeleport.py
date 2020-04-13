import fge
from fge import *

WAYPOINT_IDS = []
WAYPOINT_WEIGHTS = []
CURRENT_WAYPOINT = -1
NEXT_WAYPOINT = -1

def toggle_particles(running):
    global CURRENT_WAYPOINT
    global NEXT_WAYPOINT

    currentPos = fge.GetTransform(CURRENT_WAYPOINT).GetPosition()
    currentParticles = fge.GetParticles(CURRENT_WAYPOINT)
    currentParticles.SetPosition(currentPos[0], currentPos[1], currentPos[2])
    currentParticles.SetRunning(running)

    nextPos = fge.GetTransform(NEXT_WAYPOINT).GetPosition()
    nextParticles = fge.GetParticles(NEXT_WAYPOINT)
    nextParticles.SetPosition(nextPos[0], nextPos[1], nextPos[2])
    nextParticles.SetRunning(running)

def get_next():
    global WAYPOINT_IDS
    global WAYPOINT_WEIGHTS
    global CURRENT_WAYPOINT

    currentWaypoint = fge.GetGridWaypoint(CURRENT_WAYPOINT)
    waypointIDs = fge.GetEntities("GridWaypoint")
    nextWaypoint = -1
    nextOrder = -1
    minOrder = -1
    minWaypoint = -1
    currentOrder = -1
    if (currentWaypoint is not None):
        currentOrder = currentWaypoint.GetOrder()
    atMax = True
    isRandom = False
    for waypointID in waypointIDs:
        waypoint = fge.GetGridWaypoint(waypointID)
        order = waypoint.GetOrder()

        isRandom = order < 0
        if (isRandom):
            nextWaypoint = -1
            break
        if (order < minOrder or minOrder == -1):
            minOrder = order
            minWaypoint = waypointID
        if (order > currentOrder and (nextOrder == -1 or order < nextOrder)):
            nextOrder = order
            nextWaypoint = waypointID
            atMax = False

    if (not isRandom):
        if (atMax):
            nextWaypoint = minWaypoint
    else:
        validIndices = []

        numWaypoints = len(WAYPOINT_IDS)
        if (numWaypoints > 1):
            for i in range(numWaypoints):
                waypointID = WAYPOINT_IDS[i]
                if CURRENT_WAYPOINT == waypointID:
                    WAYPOINT_WEIGHTS[i] = 0.5 * WAYPOINT_WEIGHTS[i]
                else:
                    WAYPOINT_WEIGHTS[i] = 2.0 * WAYPOINT_WEIGHTS[i]
                    if (fge.IsEnabled(waypointID)):
                        validIndices.append(i)

            if (len(validIndices) > 0):
                weightSum = 0.0
                for index in validIndices:
                    weightSum = weightSum + WAYPOINT_WEIGHTS[index]
	       
                randomNum = weightSum * RNG.GetInstance().GenerateFloat(0.0, 1.0)
                selectionIndex = 0
                comparisonSum = WAYPOINT_WEIGHTS[validIndices[selectionIndex]]
                while randomNum > comparisonSum:
                    selectionIndex = selectionIndex + 1
                    comparisonSum = comparisonSum + WAYPOINT_WEIGHTS[validIndices[selectionIndex]]
                nextWaypoint = WAYPOINT_IDS[validIndices[selectionIndex]]
        else:
            waypointID = WAYPOINT_IDS[0]
            if (fge.IsEnabled(waypointID)):
                nextWaypoint = waypointID

    return nextWaypoint

def teleport(id):
    global NEXT_WAYPOINT

    entityTransform = fge.GetTransform(id)
    nextPos = fge.GetTransform(NEXT_WAYPOINT).GetPosition()
    entityTransform.SetPosition(nextPos[0], nextPos[1] + 0.01, nextPos[2])

def on_init(id):
    global WAYPOINT_WEIGHTS
    global CURRENT_WAYPOINT
    global NEXT_WAYPOINT
    global WAYPOINT_IDS

    ob = fge.GetObserver(id)
    ob.SetValue("time", 0.0)

    WAYPOINT_IDS = fge.GetEntities("GridWaypoint", True)
    numWaypoints = len(WAYPOINT_IDS)
    WAYPOINT_WEIGHTS.clear()
    for i in range(numWaypoints):
        WAYPOINT_WEIGHTS.append(1.0)
    
    if (numWaypoints > 0):
        CURRENT_WAYPOINT = -1
        CURRENT_WAYPOINT = get_next()
        NEXT_WAYPOINT = get_next()

        entityTransform = fge.GetTransform(id)
        currentPos = fge.GetTransform(CURRENT_WAYPOINT).GetPosition()
        entityTransform.SetPosition(currentPos[0], currentPos[1] + 0.01, currentPos[2])

        waypoint = fge.GetGridWaypoint(CURRENT_WAYPOINT)
        if (waypoint is not None):
            ob.SetValue("time", RNG.GetInstance().GenerateInteger(waypoint.GetMinDuration(), waypoint.GetMaxDuration()))

def on_event(id,event):
    pass

def on_update(id, dt):
    global CURRENT_WAYPOINT
    global NEXT_WAYPOINT
    
    ob = fge.GetObserver(id)
    time = ob.GetValue("time")
    time = time - dt*.001

    if ((time < 10.0) and (CURRENT_WAYPOINT != NEXT_WAYPOINT)):
        toggle_particles(True)
    if (time < 0.0):
        if (CURRENT_WAYPOINT != NEXT_WAYPOINT):
            toggle_particles(False)
            teleport(id)

        CURRENT_WAYPOINT = NEXT_WAYPOINT
        NEXT_WAYPOINT = get_next()

        waypoint = fge.GetGridWaypoint(CURRENT_WAYPOINT)
        if (waypoint is not None):
            time = RNG.GetInstance().GenerateInteger(waypoint.GetMinDuration(), waypoint.GetMaxDuration())
        else:
            time = 0.0

    ob.SetValue("time", time)