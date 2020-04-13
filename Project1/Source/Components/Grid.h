#pragma once

#include "Component.h"

/*!
 * Manages the Grid dependent state
 * variables
 *
 */
class Grid : public Component
{
public:
	/*!
	 * Grid Default Constructor
	 *
	 */
	GEAR_API Grid();
	/*!
	 * Grid Default Destructor
	 *
	 */
	GEAR_API virtual ~Grid();
	/*!
	 * Resets the Grid to its default
	 * state
	 *
	 */
	GEAR_API void Reset();
	/*!
	 * Creates a pointer to a new Grid
	 *
	 * \return A pointer to a new Grid
	 */
	GEAR_API Grid* Create();
	/*!
	 * Check if the items in the grid cells match
	 * the current recipe
	 *
	 * \param a_deltaTime The time between frames
	 */
	GEAR_API void Update(float a_deltaTime);
	/*!
	 * Display the members of the Grid so that they
	 * are modifiable through ImGui
	 *
	 */
	GEAR_API void DisplayDebug();
	/*!
	 * Handle the given event according to its
	 * type
	 *
	 * \param a_pEvent The event being handled by the Component
	 */
	GEAR_API virtual void HandleEvent(Event* a_pEvent);
	/*!
	 * Gets the recipe of the given game
	 *
	 * \param a_game Integer representing the game whose recipes is being requested
	 * \return The recipe of the given game
	 */
	GEAR_API std::vector<int> GetRecipe(int a_game);
	/*!
	 * Gets the current recipe assigned to the grid
	 *
	 * \return The current recipe assigned to the grid
	 */
	GEAR_API int GetCurrentRecipe();
	GEAR_API void Clone(int objID) {}

private:
	std::vector<std::vector<int>> mRecipeMap;
	std::vector<bool> mMatchList;
	int mCurrentRecipe;

	int mPlayerID;

	//TEST
	std::map<char, std::list<std::vector<int >>> recipeMap;/*!<    */
	std::vector<std::vector<int>> RecipeList;

};

//// create our map
//Map<String, List<Person>> peopleByForename = new HashMap<>();
//
//// populate it
//List<Person> people = new ArrayList<>();
//people.add(new Person("Bob Smith"));
//people.add(new Person("Bob Jones"));
//peopleByForename.put("Bob", people);
//
//// read from it
//List<Person> bobs = peopleByForename["Bob"];
//Person bob1 = bobs[0];
//Person bob2 = bobs[1];