#pragma once

#include "Component.h"

enum PlayMode
{
	COMMAND,
	START_DURATION,
	END_DURATION,
	END_ANIMATION
};

/*!
 * Manages/searches for references to a set
 * of Entities that the Entity is searching
 * for and has found
 *
 */
class AffineSequence : public Component
{
public:
	/*!
	 * AffineSequence Default Constructor
	 *
	 */
	GEAR_API AffineSequence();
	/*!
	 * AffineSequence Default Destructor
	 *
	 */
	GEAR_API virtual ~AffineSequence();
	/*!
	 * Resets the AffineSequence to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new AffineSequence
	 *
	 * \return A pointer to a new AffineSequence
	 */
	GEAR_API AffineSequence* Create();
	/*!
	 * Update animation timer and change animation
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the AffineSequence so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the AffineSequence with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the AffineSequence data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the AffineSequence data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the AffineSequence data is being saved to
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API void DeSerialize(Json::Value& a_root, bool a_ignoreDefault = false);
	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 * Play the current animation in the sequence
	 * and setup for the next animation
	 *
	 */
	GEAR_API void PlayCurrent();
	/*!
	 * Get the dutation of the current animation
	 *
	 * \return The duration of the current animation
	 */
	GEAR_API float GetDuration();
	/*!
	 * Get the amount of time the current animation
	 * has been active
	 *
	 * \return The amount of time the current animation has been active
	 */
	GEAR_API float GetTimer();
	/*!
	 * Get the name of the current animation
	 *
	 * \return The name of the current animation
	 */
	GEAR_API std::string GetAnimation();
	/*!
	 * Unpauses the timer
	 *
	 */
	GEAR_API void ResumeTimer();
	/*!
	 * Pauses the timer
	 *
	 */
	GEAR_API void PauseTimer();
	/*!
	 * Resets the timer to 0
	 *
	 */
	GEAR_API void ResetTimer();

	GEAR_API void Clone(int objID);

private:
	void Play();
	void Step();

private:
	std::vector<std::pair<std::string, float>> mSequence;
	int mIndex;
	float mTimer;
	float mTimerStart;
	PlayMode mMode;

	bool mPauseTimer;
	std::vector<std::string> mModeStrings;
};

