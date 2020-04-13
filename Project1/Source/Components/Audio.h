#pragma once

#include "Component.h"
#include "Transform.h"
/*!
 * \class Audio
 *  \brief Manages the calls to the adio manager for a specific set of sounds
 *
 */
class Audio : public Component {

public:
	GEAR_API Audio();
	GEAR_API ~Audio();
	GEAR_API void Update(float);
	GEAR_API void Reset();
	GEAR_API Audio * Create();
	/*!
	 * 
	 * 
	 * \param a_pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 *
	 *
	 */
	GEAR_API void Load(std::string a_sound, bool a_isLoop, bool a_is3D);

	/*!
	 *
	 *
	 */
	GEAR_API void Play();

	/*!
	 * param _times number of times to loop, -1 = infinite
	 *
	 */
	GEAR_API void Play(int _times);

	GEAR_API void PlayLoop();

	/*!
	 From the prerecorded ChannelID, it stops
	 *
	 */
	GEAR_API void Stop();

	/*!
	 *
	 *
	 */
	GEAR_API void Pause();

	/*!
	 *
	 *
	 */
	GEAR_API void Mute();
	/*!
	 * 
	 * 
	 * \param 
	 */
	GEAR_API void FadeOut(float);
	/*!
	 * 
	 * 
	 * \param 
	 */
	GEAR_API void FadeIn(float);
	/*!
	 *
	 *
	 */
	GEAR_API void Volume(float);

	/*!
	 *
	 *
	 */
	GEAR_API void TogglePause();

	/*!
	* Call this to read all of the values of the collider
	* to the json indicated with the input
	* \param the root for reading the component
	*/
	GEAR_API void Serialize(Json::Value);
	/*!
	 * Call this to write all of the values of the collider
	 * to the json indicated with the input
	 * \param the root for writing
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	*/
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);

	GEAR_API void DisplayDebug();

	GEAR_API void Clone(int objID);

private:

public:
	std::string  mSoundName;/*<! audioFile name */
	float mVolume;
	bool mStartPlaying = false;/*<! If audio need to be reproduced from its instantiation, then true */
	int mChannelID;/*<! reference to the channel to modify,  */
	bool m3D_sound;
private:
	
	Transform* _transform;
	bool mLoop;

};