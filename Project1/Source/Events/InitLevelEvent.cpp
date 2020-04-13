#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "InitLevelEvent.h"

InitLevelEvent::InitLevelEvent() : Event("InitLevelEvent")
{
	mTime = 0.0f;
	mMusic = "";
	mNumPlayers = -1;
	mLevelName = "";
}

InitLevelEvent::~InitLevelEvent()
{

}

void InitLevelEvent::Reset()
{
	mTime = 0.0f;
	mMusic = "";
	mGames.clear();
	mGrading.clear();
	mNumPlayers = -1;
	mLevelName = "";


	Event::Reset();
}

InitLevelEvent* InitLevelEvent::Create()
{
	return new InitLevelEvent();
}