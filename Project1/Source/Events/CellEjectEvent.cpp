#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "CellEjectEvent.h"

CellEjectEvent::CellEjectEvent() : Event("CellEjectEvent")
{

}

CellEjectEvent::~CellEjectEvent()
{

}

CellEjectEvent* CellEjectEvent::Create()
{
	return new CellEjectEvent();
}
