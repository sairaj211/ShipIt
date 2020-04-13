#include "pch.h"
#include "Managers/Log.h"

#include "Core.h"
#include "Audio.h"

#include "Managers/InputManager.h"
#include "Managers/EntityManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/AudioManager.h"
#include "Managers/ComponentManager.h"
#include "Entity.h"
#include "Managers/ResourceManager.h"
#include "Resource.h"
#include "Resources/JsonData.h"
#include "Engine.h"
#include "Resources/JsonData.h"
#include "Bullet.h"


Audio::Audio() : Component("Audio") {
	mLoop = false;
	m3D_sound = false;
	mVolume = 1.0f;
}
Audio::~Audio() {}

void  Audio::Update(float _deltaTime)
{

}

Audio *Audio::Create() {
	return new Audio();
}

void Audio::Reset()
{
	//if(mSoundName == "1test.wav")
		AUDIO_MANAGER->Unload(mSoundName);
		Stop();

	mSoundName = "";
	mChannelID = 0;
	mStartPlaying = false;
	mVolume = 1.0f;
	_transform = nullptr;
	mLoop = false;
	m3D_sound = false;

	Component::Reset();

}

/*!
	 * Play  sound and stores ChannelID reference  from the audio manager
*/
void Audio::Play()
{

	_transform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");
	if (!_transform)
		_transform = COMPONENT_MANAGER->GetRuntimeComponent<Transform>(mOwnerID, "Transform");
	mChannelID = AUDIO_MANAGER->Play(mSoundName, _transform->GetPosition(), m3D_sound);
	Volume(mVolume);
}



/*!
	 * Play loop  sound  _ntimes if _time = -1 then infinite loop
*/
void Audio::Play(int _times)
{
	_transform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");

	//mChannelID = AUDIO_MANAGER->Play(mSoundName, -1, _transform->mPosition);
	mChannelID = AUDIO_MANAGER->Play(mSoundName,_times, _transform->GetPosition(), m3D_sound);

	Volume(mVolume);
}

/*!
	 * Play loop sound infinite 
*/
void Audio::PlayLoop() 
{
	_transform = COMPONENT_MANAGER->GetComponent<Transform>(mOwnerID, "Transform");

	mChannelID = AUDIO_MANAGER->Play(mSoundName, -1, _transform->GetPosition(), m3D_sound);
	Volume(mVolume);
}

void Audio::Stop()
{
	AUDIO_MANAGER->Stop(mChannelID);
}

void Audio::Mute()
{
	AUDIO_MANAGER->Mute(mChannelID);
}

void Audio::TogglePause()
{
	AUDIO_MANAGER->TogglePause(mChannelID);
}

GEAR_API void Audio::Serialize(Json::Value a_root) 
{
	Json::Value::Members componentNames = a_root.getMemberNames();

	Json::Value dataRoot = a_root["mSoundName"];
	if (!dataRoot.isNull())
	{
		mSoundName = dataRoot.asString();
	}

	dataRoot = a_root["mis3D"];
	if (!dataRoot.isNull())
	{
		m3D_sound = dataRoot.asBool();
	}

	dataRoot = a_root["Loop"];
	if (!dataRoot.isNull())
	{
		mLoop = dataRoot.asBool();
	}

	if (mSoundName == "GameMaker03-01.mp3")
		LOG_INFO("STOP");

	Load(mSoundName, mLoop, m3D_sound);

	dataRoot = a_root["mVolume"];
	if (!dataRoot.isNull())
		mVolume = dataRoot.asFloat();

	dataRoot = a_root["mStartPlaying"];
	if (!dataRoot.isNull())
		mStartPlaying = dataRoot.asBool();
	 
	AUDIO_MANAGER->Load(mSoundName, m3D_sound);
}
/*!
 * Call this to write all of the values of the collider
 * to the json indicated with the input
 * \param the root for writing
*/
GEAR_API void Audio::DeSerialize(Json::Value& a_root, bool a_ignoreDefault)
{
	std::string dataFile = EntityManager::GetInstance()->GetEntity(mOwnerID, true)->GetDataFile();
	Json::Value defaultRoot = ResourceManager::GetInstance()->LoadJsonData(dataFile)->mRoot;
	if (!mLoop || a_ignoreDefault) {
		if (mSoundName != defaultRoot["mSoundName"].asString() || a_ignoreDefault)
			a_root["mSoundName"] = mSoundName;
	}

	if (mLoop != defaultRoot["Loop"].asBool() || a_ignoreDefault)
		a_root["Loop"] = mLoop;

	if (mVolume != defaultRoot["mVolume"].asFloat() || a_ignoreDefault)
		a_root["mVolume"] = mVolume;

	if (mStartPlaying != defaultRoot["mStartPlaying"].asBool() || a_ignoreDefault)
		a_root["mStartPlaying"] = mStartPlaying;
}

GEAR_API void Audio::DisplayDebug() 
{
	ImGui::Text("mSoundName", &mSoundName, 0.35f);
	ImGui::DragFloat("Volume", &mVolume, 0.35f);
	ImGui::Checkbox("mStartPlaying", &mStartPlaying);
}

/*!
	 * Play  sound and stores ChannelID reference  from the audio manager
*/
GEAR_API void Audio::HandleEvent(Event * a_pEvent)
{
	
}
void Audio::Load(std::string a_sound, bool a_isLoop, bool a_is3D)
{
	mSoundName = a_sound;
	m3D_sound = a_is3D;
	if (a_is3D)
	{
		if (a_isLoop) {
			mLoop = a_isLoop;
			AUDIO_MANAGER->LoadLoop(a_sound, a_is3D);
		}
		else
			AUDIO_MANAGER->Load(a_sound, a_is3D);
	}
	else
	{
		if (a_isLoop) {
			mLoop = a_isLoop;
			AUDIO_MANAGER->LoadLoop(a_sound, a_is3D);
		}
		else {
			AUDIO_MANAGER->Load(a_sound, a_is3D);
		}
	}


// 	if (mStartPlaying && (!a_isLoop))
// 		Play();
// 	if (mStartPlaying && (a_isLoop))
// 		PlayLoop();
}

void Audio::FadeOut(float)
{

}

void Audio::FadeIn(float)
{

}


void Audio::Volume(float a_aV)
{
	AUDIO_MANAGER->Volume(mChannelID, a_aV);
}


GEAR_API void Audio::Clone(int objID)
{
	Audio *rhs = static_cast<Audio*>(COMPONENT_MANAGER->GetComponent<Audio>(objID, "Audio"));

	mSoundName = rhs->mSoundName;
	mVolume = rhs->mVolume;
	mStartPlaying = rhs->mStartPlaying;
	m3D_sound = rhs->m3D_sound;

	_transform = rhs->_transform;
	mLoop = rhs->mLoop;
}
