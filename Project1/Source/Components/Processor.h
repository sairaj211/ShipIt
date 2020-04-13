#pragma once

#include "Component.h"

enum ProcessorState
{
	WORKING,
	IDLE,
	PAUSED,
	DONE
};

/*!
 * Manages the Processor dependent
 * state variables
 *
 */
class Processor : public Component
{
public:
	/*!
	 * Processor Default Constructor
	 *
	 */
	GEAR_API Processor();
	/*!
	 * Processor Default Destructor
	 *
	 */
	GEAR_API virtual ~Processor();
	/*!
	 * Resets the Processor to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new Processor
	 *
	 * \return A pointer to a new Processor
	 */
	GEAR_API Processor* Create();
	/*!
	 * Increments the Processor time
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Processor so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Initialize the Processor with the data
	 * stored in the given Json data
	 *
	 * \param a_root The root of the Json file that contains the Processor data
	 */
	GEAR_API void Serialize(Json::Value a_root);
	/*!
	 * Save the Processor data into the given Json
	 * root
	 *
	 * \param a_root The root of the Json file that the Processor data is being saved to
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
	 * Gets the maximum number of inputs
	 * that this processor supports at one
	 * time
	 *
	 * \return The maximum number of inputs that this processor supports at one time
	 */
	GEAR_API int GetMaxInput();
	/*!
	 * Gets a list of pairs that describe a recipe
	 * and its output
	 *
	 * \return A list of pairs that describes a recipe and its output
	 */
	GEAR_API std::vector<std::pair<int, std::unordered_map<int, int>>> GetRecipes();
	/*!
	 * Gets a flag that indicates whether the processor
	 * is currently idle
	 *
	 * \return A flag that is true if the Processor is currently idle
	 */
	GEAR_API bool IsIdle();
	/*!
	 * Gets a flag that indicates whether the processor
	 * is currently working
	 *
	 * \return A flag that is true if the Processor is currently working
	 */
	GEAR_API bool IsWorking();
	/*!
	 * Gets a flag that indicates whether the processor
	 * is currently done
	 *
	 * \return A flag that is true if the Processor is currently done
	 */
	GEAR_API bool IsDone();
	/*!
	 * Gets a flag that indicates whether the processor
	 * is currently paused
	 *
	 * \return A flag that is true if the Processor is currently paused
	 */
	GEAR_API bool IsPaused();
	/*!
	 * Starts the Processor so that the current
	 * time starts incrementing
	 *
	 */
	GEAR_API void Start();
	/*!
	 * Stops the Processor so that the current
	 * time stops incrementing and is reset
	 *
	 */
	GEAR_API void Stop();
	/*!
	 * Pauses the Processor so that the current
	 * time stops incrementing
	 *
	 */
	GEAR_API void Pause();
	/*!
	 * Resumes the Processor so that the current
	 * time starts incrementing from where it left
	 * off
	 *
	 */
	GEAR_API void Resume();
	/*!
	 * Gets the amout of time it takes for the
	 * Processor to finish processing 
	 *
	 * \return The amount of time it takes for the Processor to finish processing
	 */
	GEAR_API float GetMaxProcessingTime();
	/*!
	 * Gets the current amount of time the 
	 * Processor has spent processing
	 *
	 * \return The current amount of time the Processor has spent processing
	 */
	GEAR_API float GetCurrentProcessingTime();
	/*!
	 * Sets the current processing time to
	 * the specified time
	 *
	 * \param a_time The time that the current processing time is being set to
	 */
	GEAR_API void SetCurrentTime(float a_time);

	GEAR_API void Clone(int objID);
private:
	int mMaxInput;
	std::vector<std::pair<int, std::unordered_map<int, int>>> mRecipes;
	ProcessorState mCurrentState;
	float mMaxProcessingTime;
	float mCurrentProcessingTime;
	std::vector<std::vector<int>> mRecipeMap;
};

