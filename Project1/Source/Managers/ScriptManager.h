#pragma once

using namespace std::filesystem;
/*!
 * This manager checks the scripts that have currently been stored for any updates
 * sending an event if an update has been detected
 */
class ScriptManager
{
public:

	/*!
	 * Initialize the ScriptManager must be called before use
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Update scrapes all of the files in mFileWrites checking to 
	 * see if any files have been updated 
	 * 
	 * Fires a UpdateScriptEvent and stores the new file write time
	 * 
	 * \param[in] dt delta time that has passed
	 */
	GEAR_API void Update( float dt);
	/*!
	 * Clear the mFileWrites
	 * 
	 */
	GEAR_API void CleanUp();
	/*!
	 * Get the singleton instance of ScriptManager
	 * 
	 * \return The instance of ScriptManager
	 */
	GEAR_API static ScriptManager* GetInstance();
	/*!
	 * Adds the Script to the mFileWrites
	 * 
	 * \param filePath the path to the scrip to be run
	 */
	GEAR_API void StoreScript(std::string filePath);
private:
	GEAR_API ScriptManager();
	GEAR_API ~ScriptManager();
private:
	static ScriptManager* mInstance;/*!<    */
	std::unordered_map<std::string, file_time_type> mFileWrites;/*!<    */
};
