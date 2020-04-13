/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
- End Header --------------------------------------------------------*/

#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "FrameRateManager.h"



FrameRateManager* FrameRateManager::mInstance = nullptr;

FrameRateManager::FrameRateManager()
{
	
}


FrameRateManager::~FrameRateManager()
{
}

void FrameRateManager::FrameStart() {
	mTickStart = SDL_GetTicks();
}

void FrameRateManager::FrameEnd() {
	mTickEnd = SDL_GetTicks();

	while (mTickEnd - mTickStart < mNeededTicksPerFrame) {
		mTickEnd = SDL_GetTicks();
	}

	mFrameTime = mTickEnd - mTickStart;
}

double FrameRateManager::GetFrameTime() {
	return mFrameTime;
}

FrameRateManager * FrameRateManager::GetInstance()
{
	if (!mInstance) {
		mInstance = new FrameRateManager();
	}
	return mInstance;
}

void FrameRateManager::Cleanup() {
	delete mInstance;
}

void FrameRateManager::SetMaxFrameRate(Uint32 aMaxFrameRate)
{
	LOG_INFO("FrameRateManager Init");
	mTickEnd = mTickStart = mFrameTime = 0;

	if (0 != aMaxFrameRate) {
		mNeededTicksPerFrame = 1000 / aMaxFrameRate;
	}
	else
		mNeededTicksPerFrame = 0;
}
