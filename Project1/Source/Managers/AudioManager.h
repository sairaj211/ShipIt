#pragma once
#include "fmod/fmod.hpp"

#define AUDIO_MANAGER AudioManager::GetInstance()

namespace py = pybind11;
using namespace std;

typedef std::map<std::string, FMOD::Sound*> SoundMap;
typedef map<int, FMOD::Channel*> ChannelMap;

/*!
 *
 *
 */
class AudioManager {
public:
	/*!
	 *
	 *
	 * \return
	 */
	GEAR_API static AudioManager* GetInstance();
	/*!
	 * Initialize FMOD
	 *
	 */
	GEAR_API void Init();
	/*!
	 * Clears all unused channels
	 *
	 * \param elapsed
	 */
	GEAR_API void Update(float elapsed);
	/*!
	 * A sound that already is playing,  modify repetitions
	 *
	 * \param channelID
	 * \param times
	 */
	GEAR_API void DefineLoop(int channelID, int times);
	/*!
	 * ask FMOD actual status, to see possible errors
	 *
	 */
	GEAR_API int ErrorCheck(FMOD_RESULT result);
	GEAR_API void LevelShiftChanges();
	/*!
	 * load a sound
	 * \param sound
	 */
	GEAR_API void Load(string sound, bool a_is3D);
	/*!
	 * load sound and make it loopable
	 *
	 * \param path
	 */
	GEAR_API void LoadLoop(string path, bool a_is3D);
	/*!
	 *
	 *
	 * \param channelID
	 */
	GEAR_API void Mute(int channelID);
	/*!
	 *
	 *
	 * \param channelID
	 */
	GEAR_API void Pause(int channelID);
	/*!
	 *
	 *
	 */
	GEAR_API void PauseAll();
	GEAR_API void UnPauseAll();

	/*!
	 * One shoot play
	 *
	 * \param path
	 * \return
	 */
	GEAR_API int Play(string path,glm::vec3 pos, bool a_is3D);
	/*!
	 * possible many loops -1 =infinite,(for this , the audio must be previously Loaded as "LoadLoop")
	 *
	 * \param path
	 * \param loops number of loops to play -1 is infinite
	 * \return
	 */
	GEAR_API int Play(string path, int loops, glm::vec3, bool a_is3D);
	/*!
	 * Same sound. varies volume and pitch
	 *
	 * \param path
	 * \param minVolume
	 * \param maxVolume
	 * \param minPitch
	 * \param maxPitch
	 */
	GEAR_API void PlaySFX(const std::string& path, float minVolume, float maxVolume, float minPitch, float maxPitch);
	/*!
	 *
	 *
	 * \param channelID
	 */
	GEAR_API void Resume(int channelID);
	/*!
	 * locate 2Dsound left or right  (-1,1)
	 *
	 * \param channelID
	 * \param fPan
	 */
	GEAR_API void SetPan(int channelID, float fPan);
	/*!
	 *
	 *
	 * \param channelID
	 */
	GEAR_API void Stop(int channelID);
	/*!
	 *
	 *
	 */
	GEAR_API void StopAll();
	/*!
	 * uses original location of sound, less memory, more process
	 *
	 * \param path
	 */
	GEAR_API void Stream(string path);
	/*!
	 * Pause/resume a sound
	 *
	 * \param channelID
	 */
	GEAR_API void ToggleAllPause(int channelID);
	GEAR_API void ToggleAll();//Pause/resume every sound 
	/*!
	 *
	 *
	 */
	GEAR_API void ToggleMuteAll();
	/*!
	 *
	 *
	 */
	GEAR_API void ToggleMuteBG();
	/*!
	 * Pause/resume a sound
	 *
	 * \param channelID
	 */
	GEAR_API void TogglePause(int channelID);
	/*!
	 * release sound from memory
	 *
	 * \param path
	 */
	GEAR_API void Unload(string path);
	/*!
	 *
	 *
	 * \param channelID
	 * \param value [0..1]
	 */
	GEAR_API void Volume(int channelID, float value);
	/*!
	 *
	 *
	 * \return
	 */
	GEAR_API SoundMap* GetSoundMap() { return &sounds; }
	/*!
	 *
	 *
	 * \return
	 */
	GEAR_API void FadeOut(float, int);

	/*!
 *
 *
 * \return
 */
	GEAR_API void FadeIn(float, int);
	/*!
 *
 *
 * \return
 */
	GEAR_API void CleanUp();
	void SetMinMaxSoundDistance(FMOD::Channel &channel_id, float min_distance, float max_distance);

	GEAR_API void SetAllVolume(float value);

	GEAR_API void initializeFmod3D();
	GEAR_API bool loadSound3D(string fileName, bool stream);
	GEAR_API void Play3D();
	GEAR_API void UpdateSound3D(glm::vec3 position, glm::vec3 velocity);
	GEAR_API void UpdateListener3D(glm::vec3  position, glm::vec3 velocity, glm::vec3  forward, glm::vec3  up);
	FMOD_VECTOR VectorToFmod(const glm::vec3 position);

	GEAR_API float GetCurrentVolume();

	GEAR_API inline bool GetMute() { return isAllMute; }
	GEAR_API void SetMute(bool value);

	
private:
	GEAR_API AudioManager();
	GEAR_API ~AudioManager();
	enum FadeState { FADE_NONE, FADE_IN, FADE_OUT };
	FadeState fade;

public:

	std::vector<int> mLoadLoopChannels;/*!<    */
private:
	static AudioManager* mInstance;/*!<    */
	GEAR_API void LoadOrStream(const std::string& path, bool stream, bool a_is3D);
	FMOD::System* system;/*!<  FMODE HEART, FROM THIS, ALL CONTROL IS EXECUTED  */
	SoundMap sounds;/*!<  here the audio files are stored  */
	ChannelMap channels;/*!<  slots to play simultaneously the sounds on distinct channels  */
	int numberOfChannels = 64;/*!<    */
	int mnNextChannelId = 0;/*!<    */
	bool pauseEnabled = false; /*!<  use for toggling all sounds to only a background sound  */
	float mCurrentVolume;
	bool isAllMute;

};
