#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "LevelCompleteEvent.h"

 LevelCompleteEvent::LevelCompleteEvent() : Event("LevelCompleteEvent")
{
	mScore = 0;
	mGrade = "";
}

 LevelCompleteEvent::~LevelCompleteEvent()
{

}

 void LevelCompleteEvent::Reset()
{
	Event::Reset();
}

 LevelCompleteEvent* LevelCompleteEvent::Create()
{
	return new LevelCompleteEvent();
}
