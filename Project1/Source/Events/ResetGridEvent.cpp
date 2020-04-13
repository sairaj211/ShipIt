#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "ResetGridEvent.h"

ResetGridEvent::ResetGridEvent() : Event("ResetGridEvent")
{

}

ResetGridEvent::~ResetGridEvent()
{

}

ResetGridEvent* ResetGridEvent::Create()
{
	return new ResetGridEvent();
}
