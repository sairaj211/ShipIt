#pragma once

#define OBJECT_FACTORY ObjectFactory::GetInstance()

/*!
 * Loads and saves levels
 * 
 */
class ObjectFactory
{
public:
	/*!
	 * Get the ObjectFactory Singleton
	 * reference
	 * 
	 * \return The ObjectFactory reference
	 */
	GEAR_API static ObjectFactory* GetInstance();
	/*!
	 * Loads the specified level
	 * 
	 * \param a_fileName The name of the level file to be loaded
	 */
	GEAR_API void LoadLevel(std::string a_fileName);
	/*!
	 * Loads Entities specified in the given root
	 *
	 * \param a_root The root that specifies the Entities to be loaded
	 * \param a_isRuntime A flag indicating whether or not the Entity is being serialized during runtime
	 * \param a_parentID An Entity ID that corresponds to the parent Entity of all the loaded Entites
	 */
	GEAR_API std::vector<int> LoadEntities(Json::Value a_root, bool a_isRuntime, int a_parentID = -1, std::string a_scene= "");
	/*!
	 * Saves the level into the specified
	 * file
	 * 
	 * \param a_fileName The name of the file that the level is being saved to
	 */
	GEAR_API void SaveLevel(std::string a_fileName);
	/*!
	 * Deletes the ObjectFactory Singleton
	 * 
	 */
	GEAR_API void Cleanup();

private:
	GEAR_API ObjectFactory();
	GEAR_API ~ObjectFactory();
	GEAR_API void LoadInnerReferences(bool a_isRuntime);

private:
	static ObjectFactory* mInstance;
public:
	std::string mCurrentLevel;
};