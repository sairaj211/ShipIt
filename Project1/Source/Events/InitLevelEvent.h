#pragma once
#include "Event.h"

/*!
 * An event for when a level is initialized
 *
 */
class InitLevelEvent : public Event
{
public:
	/*!
	 * InitLevelEvent Default Constructor
	 *
	 */
	GEAR_API InitLevelEvent();
	/*!
	 * InitLevelEvent Default Destructor
	 *
	 */
	GEAR_API ~InitLevelEvent();
	/*!
	 * Resets the InitLevelEvent to its default
	 * state
	 *
	 */
	GEAR_API virtual void Reset();
	/*!
	 * Creates a pointer to a new InitLevelEvent
	 *
	 * \return A pointer to a new InitLevelEvent
	 */
	GEAR_API virtual InitLevelEvent* Create();

public:
	float mTime; /*!< The amount of time the level will last */
	std::string mMusic; /*!< The name of the music file to be played in the background */
	std::vector<int> mGames; /*!< List of integers representing the games that can be requested */
	std::vector<float> mGrading; /*!< List of scores used to determine the player's grade */
	int mNumPlayers; /*!< Number of players to start the level with */
	int mLevelNumber; /*!< Current level number */
	std::string mLevelName; /*!< Name of the level */
};

