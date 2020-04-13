#pragma once
#include "Core.h"
#include <future>

struct SDL_Window;

//Manager Defines
#define MEMORY_MANAGER MemoryManager::GetInstance()
#define FRAMERATE_MANAGER FrameRateManager::GetInstance()
#define INPUT_MANAGER InputManager::GetInstance()
#define SCRIPT_MANAGER ScriptManager::GetInstance()
#define EVENT_MANAGER EventManager::GetInstance()
#define PHYSICS_MANAGER_2D PhysicsManager2D::GetInstance()
#define PHYSICS_MANAGER PhysicsManager::GetInstance()
#define LEVEL_EDITOR LevelEditor::GetInstance()
#define PARTICLE_MANAGER ParticleManager::GetInstance()
#define LOCALIZATION LocalizationManager::GetInstance()
#define RAYCAST RayCast::GetInstance()
/*!
 *
 *
 */
class Engine
{
public:
	GEAR_API Engine(std::string aInitLevel, bool,bool _release =false);
	GEAR_API ~Engine();

	/*!
	 *
	 * Initialize window and in turn calls all the initializing functions like glew, imgui, fmod init
	 */
	GEAR_API void Init();
	/*!
	 * Main Game loop 
	 *
	 */
	GEAR_API void Run();
	/*!
	 *
	 *
	 */
	GEAR_API void Close();
	/*!
	 * Hide the mouse cursor
	 * 
	 */
	GEAR_API static void HideCursor();
	/*!
	 * Show the cursor
	 * 
	 */
	GEAR_API static void ShowCursor();
	/*!
	 * Move the cursor to the specified location
	 * 
	 * \param a_x The new x position of the mouse
	 * \param a_y The new y positoin of the mouse
	 */
	GEAR_API static void MoveCursor(float a_x, float a_y);
	/*!
	 *
	 *
	 */
	GEAR_API inline void StopEngine() { isRunning = false; }
	/*!
	 * To initialize GLEW
	 * \return
	 */
	GEAR_API int GLEWInit();
	/*!
	 * to initialize IMGUI
	 * \return
	 */
	GEAR_API int ImGuiInit();
	/*!
	 *
	 * \return
	 */
	GEAR_API void ImGuiCleanup();
	/*!
	 *
	 * Loads the scene 
	 */
	GEAR_API void InitSceneLoad();

	GEAR_API void ConfigLoad();

	GEAR_API void ConfigSave();

	GEAR_API void SetInitLevel(std::string aLevel);

	GEAR_API static void GetWindowSize(float&, float&);

	GEAR_API void SetIsRunning(bool aIsRunning);

	GEAR_API void FlipScreenMode(int aWidth = -1, int aHeight = -1);

	SDL_Window *pWindow;/*!<    */
	static Engine* GetInstance() { return mInstance; }

private:
	/*!
	 * Initialize all the managers 
	 *
	 */
	GEAR_API void InitManagers();
	GEAR_API void HandleEvents();

	//TEMP method(remove later)
public:
	std::string mIntLevel;/*!< name of the current level to be loaded    */
	bool isEditor;
	int mWindowHeight;
	int mWindowWidth;
	int mPrevHeight;
	int mPrevWidth;
	bool mFullScreen;
	std::string mLanguage;
private:
	bool isRunning;/*!< engine running check bool   */
	void* glContext;/*!<    */
	
	SDL_Event e;
	// 
	// 	time_t timer;
	// 	struct tm s = { 0 };
	//std::vector<std::future<>> m_futures;
	bool isRelease;
	
protected:
	static	Engine* mInstance;
};


//Should be defined in prototype
Engine* CreateApplication();
