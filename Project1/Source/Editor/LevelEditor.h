#pragma once
class Component;
class Entity;
class Transform;
class MaterialEditor;
/*!
 * 
 * 
 */
class LevelEditor
{

public:
	/*!
	 * 
	 * Returns instance of level editor class
	 * \return 
	 */
	GEAR_API static LevelEditor* GetInstance();
	/*!
	 * Initializes level editor class
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * upadates level editor
	 * 
	 */
	GEAR_API void Update();
	/*!
	 * upadates level editor
	 *
	 */
	GEAR_API bool Run();
	/*!
	 * delete all the memory used by level editor class
	 * 
	 */
	GEAR_API void CleanUp();

	/*!
	 * Method to add new created object to list
	 *
	 * \param entitiy_ID
	 */
	GEAR_API void AddToRecentList(int entity_ID);
	/*!
	 * Method to add objects with similar time stamp to delete list
	 *
	 */
	GEAR_API void DeleteList();
	/*!
	 * Method to undo (delete last created object)
	 *
	 * \
	 */
	GEAR_API void Undo();
	/*!
	 * Method to duplicated current selection
	 *
	 * \param selected (int)
	 */
	GEAR_API void Duplicate(int selected);

	GEAR_API Transform* GetTransform(int a_ID);


	GEAR_API void ChangeLevel();

	GEAR_API void ShortcutKeys();

	GEAR_API void DeleteObject();

	GEAR_API void HideWindows();

	GEAR_API void Properties();

	GEAR_API void MenuBar();
	GEAR_API void Raycast();
	GEAR_API void Hierarchy();


	GEAR_API void HighLightSelected();
	GEAR_API void PlayPauseStepBegin();
	GEAR_API void PlayPauseStepEnd();



	GEAR_API inline int GetLastPairID() { return mLastPairID; }

	GEAR_API inline void SetLastPairID(int aID) { mLastPairID = aID; }

private:
	/*!
	 *
	 *
	 */
	GEAR_API LevelEditor();
	/*!
	 *
	 *
	 */
	GEAR_API ~LevelEditor();
	GEAR_API void DrawEntityNode(Entity* a_pEntity);
	GEAR_API void SavePrefabPopup();
	GEAR_API void SaveScenePopup();
	GEAR_API void OpenScenePopup();
	GEAR_API void OpenWindows();

	GEAR_API void ImGUICreateFiltered(std::string);
	GEAR_API void ImGUICreateExact(std::string);


	/*!
	 *
	 *
	 */
	static LevelEditor* mInstance;
public:
	bool isDebug;/*!< asking for debug   */
	bool mRun;/*!<is running    */
	bool prevRun;
	bool mMaterialEditorActive; /*!< Flag indicating whether or not the Material Editor is active */
	int selected;/*!<    */
	bool deleteAll;
	bool showTree;
	bool showContacts;
	static int timeStamp;


	bool mManipulate, mBoundingBox;
	bool mShowCreateWindow;
	int selected1, imguiObjID;

	//TEST TODO
	std::vector<int> mRunTimeTimeList;
	static bool mShow_PropertiesWindow;/*!<  to display properties window of selected object  */
	static bool mShow_WarningWindow;/*!<  to display warning window when saving a file */
	static bool mShow_LoadLevelWindow;/*!<  to display warning window when saving a file */
	static bool mShow_SaveLevelWindow;
	static bool mShow_ResetLevel;

private:
	bool prevIsDebug;
	bool runStep;
	bool isRun;
	bool isPause;
	bool isStep;
	bool shortcutPause = false;
	
	std::vector<Component*> componentList;/*!< vector to store components list of new created object or of current selected object   */
	Entity* currentEntity;/*!<    */
	std::vector<std::pair<int,int>> recentlyCreatedObjects;/*!<vector of pair which contains newly created objectid and timestamp    */
	std::vector<std::pair<int,Entity*>> deleteThem;/*!< vector containing entities with same timestamp to delete   */
	std::vector<Entity*> redoList;/*!<  TODO object redo list  */
	MaterialEditor* mpMaterialEditor;
	ULONG64 frame_num;
	int TRS;
	bool savedPrefab;

	template <typename T> T convert_to(const std::string &str)
	{
		std::istringstream ss(str);
		T num;
		ss >> num;
		return num;
	}


	//TELEPORT PARAMETERS
	int mLastPairID;

};