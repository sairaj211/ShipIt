/* Start Header -------------------------------------------------------
Copyright (C) 2019 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior
written consent of DigiPen Institute of Technology is prohibited.
Author: Ivan Cazares, ivan.c, 60004418
- End Header --------------------------------------------------------*/

#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "AudioManager.h"

// just provisional, must be included into a general document
string pathSound = ".\\Resources\\Audio\\";

bool isBackgroundMute = false;

float RandomBetween(float, float);
float ChangeSemitone(float, float);

AudioManager* AudioManager::mInstance = nullptr;


AudioManager::AudioManager()
{
}

//to maintain control over the function
AudioManager::~AudioManager()
{
	// Release every sound object and clear the map
	SoundMap::iterator iter;
	for (iter = sounds.begin(); iter != sounds.end(); ++iter)
		iter->second->release();
	sounds.clear();
	// Release the system object
	system->release();
	system = 0;
}

AudioManager * AudioManager::GetInstance()
{
	if (!mInstance) {
		mInstance = new AudioManager();
	}
	return mInstance;
}

void AudioManager::Init()
{
	LOG_INFO("AudioManager Init");

	mCurrentVolume = 0.1f;

	// Create FMOD System
	FMOD_RESULT errorCode = FMOD::System_Create(&system);
	if (errorCode != FMOD_OK)
		LOG_ERROR(FMOD_ErrorString(errorCode));

	// Check FMOD Version
	unsigned int version;
	system->getVersion(&version);
	if (version < FMOD_VERSION)
		LOG_ERROR("FMOD lib version doesn't match header version");

	// Initialize FMOD
	errorCode = system->init(numberOfChannels, FMOD_INIT_NORMAL, 0);
	if (errorCode != FMOD_OK)
		LOG_ERROR(FMOD_ErrorString(errorCode));

	isAllMute = false;
}

// Release resources
void AudioManager::Update(float elapsed)
{
	ToggleMuteAll();
	vector<ChannelMap::iterator> pStoppedChannels;
	bool bIsPlaying = false;
	for (auto it = channels.begin(), itEnd = channels.end(); it != itEnd; ++it) {
		it->second->isPlaying(&bIsPlaying);
		if (!bIsPlaying)
			pStoppedChannels.push_back(it);
	}
	for (auto& it : pStoppedChannels)//releases all channels that are not actually playing
		channels.erase(it);

	AudioManager::ErrorCheck(system->update());//run update and check for erors

}

void AudioManager::DefineLoop(int channelID, int times)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setLoopCount(times);
}

//ask system for problems
int AudioManager::ErrorCheck(FMOD_RESULT result)
{
	if (result != FMOD_OK) {
		LOG_ERROR(FMOD_ErrorString(result));
		return 1;
	}
	//printf("FMOD all good ");
	return 0;
}

void AudioManager::Load(string sound, bool a_is3D)
{
	LoadOrStream(sound, false, a_is3D);
}

void AudioManager::LoadLoop(std::string path, bool a_is3D)
{
	if (sounds.find(path) != sounds.end()) return;
	FMOD::Sound* sound;
	const std::string& path2 = pathSound + path;//relative to project folder
	if (a_is3D)
	{
		system->createSound(
			path2.c_str(),
			FMOD_3D | FMOD_LOOP_NORMAL,
			nullptr,
			&sound //storing address
		);
	}
	else
	{
		system->createSound(
			path2.c_str(),
			FMOD_LOOP_NORMAL,
			nullptr,
			&sound //storing address
		);
	}

	sounds.insert(std::make_pair(path, sound));// Store sound object in the map using the path as key
}

void AudioManager::LevelShiftChanges()
{
	if (isBackgroundMute) {
		isBackgroundMute = false;
		ToggleMuteBG();
	}
}

//decides if load to memory or stream directly from the source
void AudioManager::LoadOrStream(const std::string& name, bool stream, bool a_is3D)
{
	// Ignore call if sound is already loaded
	if (sounds.find(name) != sounds.end()) return;
	// Load (or stream) file into a sound object
	FMOD::Sound* sound;

	const std::string& path2 = pathSound + name;//relative to project folder
	if (stream)
		system->createStream(
			path2.c_str(),//name
			FMOD_DEFAULT,//sound nature loop, hardware accelerator,2D/3D
			nullptr, // special paramethers when loading a subsound form larger, midi dls sample set. etc,,, basically dont touch now
			&sound//
		);
	else
	{
		if (a_is3D) {
			system->createSound(
				path2.c_str(),
				FMOD_3D,
				nullptr,
				&sound //storing address
			);
		}
		else
		{
			system->createSound(
				path2.c_str(),
				FMOD_2D,
				nullptr,
				&sound //storing address
			);

		}
	}
	//AudioManager::ErrorCheck(sound->set3DMinMaxDistance(1.0f,2.0f));
	// Store the sound object in the map using the path as key
	sounds.insert(std::make_pair(name, sound));
}

void AudioManager::PauseAll()
{
	FMOD::Channel* pChannel = nullptr;
	for (int i = 0; i < numberOfChannels; i++) {
		pChannel = channels[i];
		pChannel->setPaused(true);
	}
}

GEAR_API void AudioManager::UnPauseAll()
{
	
	FMOD::Channel* pChannel = nullptr;
	for (int i = 0; i < numberOfChannels; i++) {
		pChannel = channels[i];
		pChannel->setPaused(false);
	}
}

void  AudioManager::Mute(int channelID)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setVolume(0);
}

void AudioManager::Pause(int channelID)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setPaused(true);
}

int AudioManager::Play(string path,glm::vec3 pos, bool a_is3D)
{
	ToggleMuteAll();
	int channelID = mnNextChannelId++;
	SoundMap::iterator sound = sounds.find(path);// Search for a matching sound in the map
	if (sound == sounds.end())
		return -1;// Ignore call if no sound was found

	FMOD::Channel* pChannel = nullptr;
	system->playSound(	//  play the sound
		sound->second,// 
		nullptr,//FMOD_CHANNEL_FREE select an intrenal channel
		false, //starts in pause? false? then play right now
		&pChannel//  channel reference for further manipulation. //advanced function must return the reference channel
	);
	if (pChannel) {
		channels[channelID] = pChannel;
		if (a_is3D)
		{
			AudioManager::SetMinMaxSoundDistance(*pChannel, 1.0f, 5000.0f);
			FMOD_VECTOR position = VectorToFmod(pos);
			FMOD_VECTOR vel = VectorToFmod(glm::vec3(1.0f, 0.0f, 0.0f));
			AudioManager::ErrorCheck(pChannel->set3DAttributes(&position, &vel));
		}
	}
	
	if (isAllMute)
		Mute(channelID);

	return channelID;
}

int AudioManager::Play(string path, int loops, glm::vec3 pos, bool a_is3D)//-1 infinite
{
	ToggleMuteAll();

	int channelID = mnNextChannelId++;
	SoundMap::iterator sound = sounds.find(path);// Search for a matching sound in the map
	if (sound == sounds.end())
		return -1;// Ignore call if no sound was found

	FMOD::Channel* pChannel = nullptr;
	system->playSound(	//  play the sound
		sound->second,// 
		nullptr,//FMOD_CHANNEL_FREE select an intrenal channel
		false, //starts in pause? false? then play right now
		&pChannel//  channel reference for further manipulation. //advanced function must return the reference channel
	);
	if (pChannel) {
		channels[channelID] = pChannel;
		if (a_is3D)
		{
			AudioManager::SetMinMaxSoundDistance(*pChannel, 1.0f, 5000.f);
			FMOD_VECTOR position = VectorToFmod(pos);
			FMOD_VECTOR vel = VectorToFmod(glm::vec3(1.0f, 0.0f, 0.0f));
			AudioManager::ErrorCheck(pChannel->set3DAttributes(&position, &vel));
		}
	}
	channels[channelID]->setLoopCount(loops);
	if (isBackgroundMute || isAllMute)
		Mute(channelID);

	return channelID;
}

void AudioManager::PlaySFX(const std::string& path, float minVolume, float maxVolume, float minPitch, float maxPitch)
{
	SoundMap::iterator sound = sounds.find(path);
	if (sound == sounds.end()) return;// if there is not sound, do nothing
	// Calculate random volume and pitch in selected range
	float volume = RandomBetween(minVolume, maxVolume);
	float pitch = RandomBetween(minPitch, maxPitch);
	// Play the sound effect with these initial values
	FMOD::Channel* channel;
	system->playSound(
		sound->second,
		nullptr,
		true,
		&channel
	);
	channel->setVolume(volume);
	float frequency;
	channel->getFrequency(&frequency);
	//channel->setFrequency(ChangeSemitone(frequency, pitch));
	channel->setPaused(false);

	if (isAllMute)
		channel->setVolume(0.0f);
}

void AudioManager::Resume(int channelID)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setPaused(false);
}

void AudioManager::SetPan(int channelID, float fPan)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setPan(fPan);
}

void AudioManager::Stop(int channelID)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->stop();
}

void AudioManager::StopAll()
{
	for (int i = 0; i < numberOfChannels; i++) {
		FMOD::Channel* pChannel = nullptr;
		FMOD_RESULT res = system->getChannel(i, &pChannel);
		if (res == FMOD_OK && pChannel) {
			pChannel->stop();
		}
	}
}

// void AudioManager::Stream(string path)
// {
// 	LoadOrStream(path, true);
// }

void AudioManager::ToggleAll()
{
	//Toggle all sounds in channels
	for (int i = 0; i < numberOfChannels; i++) {
		FMOD::Channel* pChannel = nullptr;
		FMOD_RESULT res = system->getChannel(i, &pChannel);
		if (res == FMOD_OK && pChannel) {
			TogglePause(i);
		}
	}
}

void AudioManager::ToggleAllPause(int channelID) {
	pauseEnabled = !pauseEnabled;
	//Toggle all sounds in channels
	for (int i = 0; i < numberOfChannels; i++) {
		FMOD::Channel* pChannel = nullptr;
		FMOD_RESULT res = system->getChannel(i, &pChannel);
		if (res == FMOD_OK && pChannel) {
			if (pauseEnabled)
				Pause(i);
			else
				Resume(i);
		}
	}
	if (!pauseEnabled)
		Pause(channelID);
	else
		Resume(channelID);
}

void AudioManager::ToggleMuteAll()
{
	if (isAllMute) {
		for (auto channel : channels) {
			Mute(channel.first);
		}
	}
	else {
		for (auto channel : channels) {
			channels[channel.first]->setVolume(GetCurrentVolume());
		}
	}

	//isAllMute = !isAllMute;
}

void AudioManager::ToggleMuteBG()
{
	if (!isBackgroundMute) {
		for (int channelID : mLoadLoopChannels) {
			Mute(channelID);
		}
	}
	else {
		for (int channelID : mLoadLoopChannels) {
			channels[channelID]->setVolume(1);
		}
	}

	isBackgroundMute = !isBackgroundMute;
}

void AudioManager::TogglePause(int channelID)
{
	bool paused;
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->getPaused(&paused);
	channels[channelID]->setPaused(!paused);
}

void AudioManager::Unload(string path)
{
	SoundMap::iterator sound = sounds.find(path);
	if (sound == sounds.end()) return;

	sounds.erase(sound);
}

void  AudioManager::Volume(int channelID, float value)
{
	FMOD::Channel* pChannel = nullptr;
	pChannel = channels[channelID];
	channels[channelID]->setVolume(value);
}


float RandomBetween(float min, float max)
{
	if (min == max) return min;
	float n = (float)rand() / (float)RAND_MAX;
	return min + n * (max - min);
}

float ChangeSemitone(float frequency, float variation)
{
	static float semitone_ratio = pow(2.0f, 1.0f / 12.0f);
	return frequency * pow(semitone_ratio, variation);
}

void AudioManager::FadeOut(float _time, int _channelID)
{

}


void AudioManager::CleanUp()
{
	delete mInstance;
	mInstance = nullptr;
}

void AudioManager::SetMinMaxSoundDistance(FMOD::Channel & channel_id, float min_distance, float max_distance)
{
	//max_distance *= 10.0f;
	channel_id.set3DMinMaxDistance(min_distance, max_distance);

}

///https://github.com/joshuajnoble/Programming-Interactivity-Code/blob/master/ch10/1005/Sound3d.cpp
void  AudioManager::initializeFmod3D() {

	//if (!bFmod3DInitialized) {
	//	FMOD_System_Create(&sys);
	//	FMOD_System_Init(sys, 32, FMOD_INIT_NORMAL, NULL);
	//	//do we want just 32 channels?

	//	FMOD_System_Set3DSettings(sys, 10.0f, 10.0f, 10.0f);
	//	FMOD_System_GetMasterChannelGroup(sys, &channelgroup);
	//	bFmod3DInitialized = true;
	//}
}
bool  AudioManager::loadSound3D(string fileName, bool stream) 
{
	bool bLoadedOk =false;
	//result = FMOD_System_CreateSound(sys, ofToDataPath(fileName).c_str(), FMOD_3D, NULL, &sound);
	//result = FMOD_Sound_Set3DMinMaxDistance(sound, 1.f, 5000.0f);

	//if (result != FMOD_OK) {
	//	bLoadedOk = false;
	//	printf("ofSoundPlayer: Could not load sound file %s \n", fileName.c_str());
	//}
	//else {
	//	bLoadedOk = true;
	//	FMOD_Sound_GetLength(sound, &length, FMOD_TIMEUNIT_PCM);
	//	isStreaming = stream;
	//}
	//return bLoadedOk;
	return false;

}
void  AudioManager::Play3D() 
{
	/*FMOD_System_PlaySound(sys, FMOD_CHANNEL_FREE, sound, bPaused, &channel);
	FMOD_VECTOR pos = { 0.0f, 0.0f, 0.0f };
	FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	FMOD_Channel_Set3DAttributes(channel, &pos, &vel);
	FMOD_Channel_GetFrequency(channel, &internalFreq);
	FMOD_Channel_SetVolume(channel, volume);*/
}
void  AudioManager::UpdateSound3D(glm::vec3 position, glm::vec3 velocity)
{
	/*soundPosition.x = position.x;
	soundPosition.y = position.y;
	soundPosition.z = position.z;

	soundVelocity.x = velocity.x;
	soundVelocity.y = velocity.y;
	soundVelocity.z = velocity.z;

	FMOD_Channel_Set3DAttributes(channel, &soundPosition, &soundVelocity);*/
}
void  AudioManager::UpdateListener3D(glm::vec3  position, glm::vec3 velocity, glm::vec3  forward, glm::vec3  up)
{

	//listenerVelocity.x = velocity.x;
	//listenerVelocity.y = velocity.y;
	//listenerVelocity.z = velocity.z;

	//listenerPos.x = position.x;
	//listenerPos.y = position.y;
	//listenerPos.z = position.z;

	//listenerForward.x = forward.x;
	//listenerForward.y = forward.y;
	//listenerForward.z = forward.z;

	//listenerUp.x = up.x;
	//listenerUp.y = up.y;
	//listenerUp.z = up.z;

	//FMOD_System_Set3DListenerAttributes(sys, 0, &listenerPos, &listenerVelocity, &listenerForward, &listenerUp);
	int listener = 0;
	system->set3DListenerAttributes(listener, &VectorToFmod(position),&VectorToFmod(velocity), &VectorToFmod(forward), &VectorToFmod(up));

}

FMOD_VECTOR AudioManager::VectorToFmod(const glm::vec3 position)
{
	FMOD_VECTOR vector;
	vector.x = position.x;
	vector.y = position.y;
	vector.z = position.z;
	return vector;
}

void AudioManager::SetAllVolume(float value)
{
	FMOD::Channel* pChannel = nullptr;
	for (int i = 0; i < channels.size(); i++) {
		pChannel = channels[i];
		if (pChannel)
		{
			pChannel->setVolume(value);
		}
	}
	if (value > 0.0f)
	{
		mCurrentVolume = value;
	}
}

float AudioManager::GetCurrentVolume()
{
	return mCurrentVolume;
}

void AudioManager::SetMute(bool value)
{
	 isAllMute = value; 
	 ToggleMuteAll(); 
}
