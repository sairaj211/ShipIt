#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "GeneratorOutputEvent.h"

GeneratorOutputEvent::GeneratorOutputEvent() : Event("GeneratorOutputEvent")
{
	mGeneratorID = -1;
	mPlayerID = -1;
	mOutputs.clear();
}

GeneratorOutputEvent::~GeneratorOutputEvent()
{

}

void GeneratorOutputEvent::Reset()
{
	mGeneratorID = -1;
	mPlayerID = -1;
	mOutputs.clear();

	Event::Reset();
}

GeneratorOutputEvent* GeneratorOutputEvent::Create()
{
	return new GeneratorOutputEvent();
}