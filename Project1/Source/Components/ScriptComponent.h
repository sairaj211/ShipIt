#pragma once
#include "Component.h"
#include <pybind11/pybind11.h>
namespace py = pybind11;



/*!
 * \class Component that manages the scripts
 * \brief Runs, updates, and initializes python scripts
 */
class ScriptComponent : public Component
{
public:
	GEAR_API ScriptComponent();
	GEAR_API ~ScriptComponent();
	/*!
	 * 
	 * 
	 * \return new script component
	 */
	GEAR_API virtual ScriptComponent* Create();
	/*!
	 * Iterate through the scripts associated with this component and 
	 * call the on_update function
	 * 
	 * \param deltaTime
	 */
	GEAR_API virtual void Update(float deltaTime);
	/*!
	 * 
	 * 
	 */
	GEAR_API virtual void DisplayDebug();
	/*!
	 * Read the scripts from the incidated root 
	 * 
	 * \param root
	 */
	GEAR_API virtual void Serialize(Json::Value root);
	/*!
	 * save the scripts to the incidated root TODO
	 * 
	 * \param root
	 * \param a_ignoreDefault A flag indicating whether or not the default prefab values are ignored
	 */
	GEAR_API virtual void DeSerialize(Json::Value& root, bool a_ignoreDefault = false);
	/*!
	 * Handle any incoming events, mainly update script
	 * 
	 * \param pEvent
	 */
	GEAR_API virtual void HandleEvent(Event* pEvent);

	/*!
	 * self reference for python references here
	 * 
	 * \return id of this component
	 */
	GEAR_API int GetId();


	/*!
	 * Call to initialize the scripts calling the on_init function
	 * in the script
	 * 
	 */
	GEAR_API virtual void Init();

	/*!
	 * THis is and exposed fucntion to the python api
	 * 
	 * \return the entity Id
	 */
	GEAR_API int GetSelf() { return mOwnerID; }
	
	GEAR_API void Reset();

	GEAR_API void Clone(int objID);

private:
	std::unordered_map<std::string,py::module> mScriptModules;//paths to the script resource to be called manager
	bool mIsAlwaysRun;
	GEAR_API void AddScript(std::string _path, bool init =false);


public:
	int current;
};

