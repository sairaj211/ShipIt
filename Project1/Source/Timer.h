#pragma once
#include <chrono>
#include <thread>

#define TIMER Timer::GetInstance()

/*!
 * A Struct defining the attributes of a
 * record for the miliseconds between two
 * lines of code
 *
 */
struct Record
{
	std::string message; /*!< Message describing the lines of code being timed */
	std::chrono::time_point<std::chrono::steady_clock> start; /*!< Start time of the timer */
	float duration; /*!< Time in miliseconds between the two lines of code */
};

/*!
 * Records and displays the miliseconds
 * between two lines of code
 *
 */
class Timer
{
public:
	/*!
	 * Get the Timer Singleton reference
	 *
	 * \return The Timer reference
	 */
	GEAR_API static Timer* GetInstance();
	/*!
	 * Draws the recorded times in miliseconds
	 *
	 */
	GEAR_API void Draw();
	/*!
	 * Start timer
	 *
	 */
	GEAR_API void Start(std::string a_message);
	/*!
	 * End current timer
	 *
	 */
	GEAR_API void End();

private:
	GEAR_API Timer();
	GEAR_API ~Timer();

private:
	static Timer* mInstance;
	std::vector<Record> mRecords;
	std::stack<int> mActiveStack;
};