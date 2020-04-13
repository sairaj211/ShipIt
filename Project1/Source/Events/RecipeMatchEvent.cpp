#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "RecipeMatchEvent.h"

RecipeMatchEvent::RecipeMatchEvent() : Event("RecipeMatchEvent")
{
	mProcessorID = -1;
	mOutput = -1;
}

RecipeMatchEvent::~RecipeMatchEvent()
{

}

void RecipeMatchEvent::Reset()
{
	mProcessorID = -1;
	mOutput = -1;

	Event::Reset();
}

RecipeMatchEvent* RecipeMatchEvent::Create()
{
	return new RecipeMatchEvent();
}