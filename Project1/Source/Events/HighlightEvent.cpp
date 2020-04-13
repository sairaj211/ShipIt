#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "HighlightEvent.h"

HighlightEvent::HighlightEvent() : Event("HighlightEvent")
{

}

HighlightEvent::~HighlightEvent()
{

}

HighlightEvent* HighlightEvent::Create()
{
	return new HighlightEvent();
}
