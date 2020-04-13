#include "pch.h"


#include "Managers/Log.h"
#include "Core.h"

#include "Engine.h"
#include "Managers/MemoryManager.h"
#include "Managers/ResourceManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/EntityManager.h"
#include "Managers/ComponentManager.h"
#include "Managers/FrameRateManager.h"
#include "Managers/InputManager.h"
#include "Managers/AudioManager.h"
#include "Managers/ShapeGenerator.h"
#include "Managers/CameraManager.h"
#include "Editor/LevelEditor.h"
#include "Editor/RayCast.h"
#include "Input/Mouse.h"

#include "Managers/ResourceManager.h"
#include "Managers/ObjectFactory.h"
#include "Managers/ComponentManager.h"
#include "Managers/EntityManager.h"
#include "Managers/MemoryManager.h"
#include "Managers/ScriptManager.h"
#include "Managers/EventManager.h"
#include "Managers/LocalizationManager.h"
#include "Managers/Renderer.h"
#include "Physics/PhysicsManager.h"
#include "Managers/ParticleManager.h"
#include "Managers/EnemyStateManager.h"
#include "Managers/SceneManager.h"

#include "Resources/JsonData.h"
#include "Entity.h"
#include "Managers/Log.h"
#include "Components/Transform.h"
#include "Components/Body.h"

#include "Random/RandomNumberGenerator.h"
#include "Random/Noise.h"

#include <stb_image.h>

#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_impl_sdl.h"

#include <time.h>
#include <ctime>


#include "Timer.h"
#include "Resources/Material.h"
#include <future>

#include <mutex>
#include <condition_variable>

#include "TestDefine.h"

Engine* Engine::mInstance = nullptr;
bool stopForNextMain = true;
bool stopForNextWorker = true;

//bool goAheadMain = true;
bool goAheadWorker = false;

//using namespace boost::python;

GLfloat deltaTime = 0.0f;
GLfloat lastFrame = 0.0f;

bool shouldGoIn = true;
bool shoudlMainThreadSleep = true;


Engine::Engine(std::string aLevel,bool editor, bool _release) :pWindow(nullptr), isRunning(true),mIntLevel(aLevel), isEditor(editor), isRelease(_release)
{
	
	Init();
	Engine::mInstance = this;
	InitManagers();
	
}

Engine::~Engine()
{

}

int Engine::GLEWInit()
{
	//Initialize GLEW
	if (glewInit() != GLEW_OK)
		printf("Error initializing GLEW\n");

	return 0;
}

int Engine::ImGuiInit()
{
	// Check ImGui Version
	if (!IMGUI_CHECKVERSION())
		return -1;

	// Initialize ImGui for OpenGL
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
	(void)io;
	ImGui_ImplSDL2_InitForOpenGL(pWindow, glContext);
	ImGui_ImplOpenGL3_Init("#version 330");
	ImGui::StyleColorsDark();

	return 0;
}

void Engine::ImGuiCleanup()
{
	// Cleanup ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void Engine::InitSceneLoad() {
// 	// Creates and loads entities and their components from specified level file
// 	SCENE_MANAGER->PushScene(mIntLevel);
// 	ObjectFactory::GetInstance()->LoadLevel(mIntLevel);
// 	ENTITY_MANAGER->ClearRuntimeLists();
// 	COMPONENT_MANAGER->ClearRuntimeLists();
// 
// 	// This is temporary
// 	// It is just initializing the scene according to a json file
// 	// Remove later.
// 	Json::Value root = RESOURCE_MANAGER->LoadJsonData("DigiPen", ".\\Resources\\LevelData\\")->mRoot;
// 	InitLevelEvent* initLevelEvent = static_cast<InitLevelEvent*>(MEMORY_MANAGER->GetEventFromMemory("InitLevelEvent"));
// 	Json::Value levelData = root["Time"];
// 	if (!levelData.isNull())
// 		initLevelEvent->mTime = levelData.asFloat();
// 	
// 	levelData = root["Games"];
// 	if (!levelData.isNull())
// 		for (unsigned int i = 0; i < levelData.size(); ++i)
// 			initLevelEvent->mGames.push_back(levelData[i].asInt());
// 
// 	// Get the number of players
// 	int numPlayers = 2;
// 	int numControllers = int(INPUT_MANAGER->GetPlayerGamePads().size());
// 	if (numControllers > 2)
// 		numPlayers = numControllers;
// 	//numPlayers = 4;
// 	levelData = root["Grading"];
// 	if (!levelData.isNull()) {
// 		Json::Value grades = levelData[numPlayers - 2];
// 		for (unsigned int i = 0; i < grades.size(); ++i)
// 			initLevelEvent->mGrading.push_back(grades[i].asFloat());
// 	}
// 
// 	initLevelEvent->mNumPlayers = numPlayers;
// 
// 	EVENT_MANAGER->BroadcastEvent(initLevelEvent);
}

GEAR_API void Engine::ConfigLoad()
{
	JsonData* pJsonData = new JsonData("config", "Config/");
	pJsonData->Load();
	Json::Value config = pJsonData->mRoot;

	Json::Value val = config["full-screen"];
	if (!val.isNull())
		mFullScreen = config["full-screen"].asBool();
	
	val = config["height"];
	if (!val.isNull()) {
		mWindowHeight = config["height"].asInt();
		mPrevHeight = mWindowHeight;
	}
	
	val = config["width"];
	if (!val.isNull()) {
		mWindowWidth = config["width"].asInt();
		mPrevWidth = mWindowWidth;
	}

	val = config["language"];
	if (!val.isNull())
		mLanguage = config["language"].asString();

	delete pJsonData;
}

GEAR_API void Engine::ConfigSave()
{
	Json::Value config = ResourceManager::GetInstance()->LoadJsonData("config", "Config/")->mRoot;


	if (config["full-screen"] != mFullScreen) 
		config["full-screen"] = mFullScreen;

	if (config["height"] != mWindowHeight)
		config["height"] = mWindowHeight;
	

	if (config["width"] != mWindowWidth)
		config["width"] = mWindowWidth;

	if (config["language"] != mLanguage)
		config["language"] = mLanguage;

	Json::StreamWriterBuilder streamBuilder;
	Json::StreamWriter* writer = streamBuilder.newStreamWriter();

	//Deserializer
	std::ofstream output("./Config/config.json");
	writer->write(config, &output);
	output.close();

}

void Engine::FlipScreenMode(int aWidth, int aHeight)
{
	if (aWidth > 0 && aHeight > 0) {
		mWindowWidth = aWidth;
		mWindowHeight = aHeight;
		SDL_SetWindowSize(pWindow, mWindowWidth, mWindowHeight);
	}
	
	if (mFullScreen == true)
		SDL_SetWindowFullscreen(pWindow, SDL_WINDOW_FULLSCREEN);
	else 
		SDL_SetWindowFullscreen(pWindow, 0);

	SDL_SetWindowPosition(pWindow, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Engine::Init()
{
	ConfigLoad();

	int error = 0;
	Log::Init();

	//Initialize SDL
	if ((error = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER | SDL_INIT_JOYSTICK)) < 0)//Init for video and joystick
	{
		printf("Couldn't initialize SDL, error %i\n", error);
		return;
	}
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	


	// Enable AntiAliasiing (Don't know what does this do)
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	
	pWindow = SDL_CreateWindow("5thGear",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		static_cast<int>(Engine::mWindowWidth),
		static_cast<int>(Engine::mWindowHeight),
		SDL_WINDOW_OPENGL);
	SDL_SetHint(SDL_HINT_ALLOW_TOPMOST, "0");
	FlipScreenMode();

	//Check if window was made successfully
	if (pWindow == NULL)
	{
		printf("Could not create window: %s\n", SDL_GetError());
		return;
	}

	glContext = SDL_GL_CreateContext(pWindow);
	if (glContext == NULL)
	{
		printf("OpenGL context could not be created. SDL Error: %s\n", SDL_GetError());
	}

#ifdef DEBUG
	ShowCursor();
#else
	ShowCursor();
#endif

}

void Engine::Run()
{
	LEVEL_EDITOR->isDebug = isEditor;
	LEVEL_EDITOR->mRun = !isEditor;
	
	while (isRunning)
	{
		//wrap in statemanager
		FRAMERATE_MANAGER->FrameStart();
		float deltaTime = (float)FRAMERATE_MANAGER->GetFrameTime();
		
		INPUT_MANAGER->Update();

		HandleEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL2_NewFrame(pWindow);
		ImGui::NewFrame();

		SCENE_MANAGER->Update();
		TIMER->Start("Scripts");
		SCRIPT_MANAGER->Update(deltaTime);
		TIMER->End();

		CAMERA_MANAGER->Update(deltaTime);

#ifdef PROTOTYPE
		ENEMY_STATE_MANAGER->Update(deltaTime);
#endif

		TIMER->Start("Physics");
		if (LEVEL_EDITOR->Run()) {
			PHYSICS_MANAGER->Update(deltaTime);
		}
		TIMER->End();

		EVENT_MANAGER->Update(deltaTime);

		if (INPUT_MANAGER->IsTriggered(SDL_SCANCODE_M)) {
			RENDERER->mAntiAliasing = !RENDERER->mAntiAliasing;
		}
		
		LEVEL_EDITOR->Update();

		AUDIO_MANAGER->Update(deltaTime);
		TIMER->Start("ComponentUpdate");
		COMPONENT_MANAGER->Update(deltaTime);
		TIMER->End();
		TIMER->Start("Renderer");
		RENDERER->Draw();
		TIMER->End();

		ENTITY_MANAGER->ClearRuntimeLists();
		COMPONENT_MANAGER->ClearRuntimeLists();
		EVENT_MANAGER->Clean();

		if (LEVEL_EDITOR->deleteAll) {
			//	ObjectFactory::GetInstance()->LoadLevel(OBJECT_FACTORY->mCurrentLevel);
			LEVEL_EDITOR->deleteAll = false;
			LEVEL_EDITOR->mRun = false;
			//SCENE_MANAGER->InitSceneLoad();
			/*ENEMY_STATE_MANAGER->Init();*/
			PauseEvent* start = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
			start->mPause = isEditor;
			EVENT_MANAGER->BroadcastEvent(start);
			RAYCAST->Init();
		}
	//	TIMER->Draw();


		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		
		// Swap the buffer
		TIMER->Start("SwapWindow");
		SDL_GL_SwapWindow(pWindow);
		TIMER->End();
		TIMER->Start("FramerateManager");
		FRAMERATE_MANAGER->FrameEnd();
		TIMER->End();

	}
}

void Engine::Close()
{
	// ImGui Cleanup
	ImGuiCleanup();
	ConfigSave();
	PARTICLE_MANAGER->CleanUp();
	PHYSICS_MANAGER->Cleanup();

	RESOURCE_MANAGER->Cleanup();
	OBJECT_FACTORY->Cleanup();
	COMPONENT_MANAGER->Cleanup();
	ENTITY_MANAGER->Cleanup();
	AUDIO_MANAGER->CleanUp();
	MEMORY_MANAGER->Cleanup();
	INPUT_MANAGER->Cleanup();
	RENDERER->Cleanup();
	SCRIPT_MANAGER->CleanUp();
	SHAPE_GENERATOR->Cleanup();
	LOCALIZATION->CleanUp();
	RNG->Cleanup();
	NOISE->Cleanup();
	SCENE_MANAGER->Cleanup();

	FRAMERATE_MANAGER->Cleanup();
	LEVEL_EDITOR->CleanUp();
	CAMERA_MANAGER->Cleanup();
	//ENEMY_STATE_MANAGER->CleanUp();

	// SDL Cleanup
	SDL_GL_DeleteContext(glContext);
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}


void Engine::HideCursor()
{
	SDL_ShowCursor(SDL_DISABLE);
}

void Engine::ShowCursor()
{
	SDL_ShowCursor(SDL_ENABLE);
}

void Engine::MoveCursor(float a_x, float a_y)
{
	SDL_WarpMouseInWindow(mInstance->pWindow, int(a_x), int(a_y));
}

void Engine::SetInitLevel(std::string aLevel)
{
	mIntLevel = aLevel;
}

GEAR_API void Engine::GetWindowSize(float&width, float&height)
{
	width = (float)mInstance->mWindowWidth;
	height = (float)mInstance->mWindowHeight;
}

void Engine::SetIsRunning(bool aIsRunning)
{
	isRunning = aIsRunning;
}

void Engine::InitManagers()
{
	GLEWInit();
	MEMORY_MANAGER->Init();
	AUDIO_MANAGER->Init();
	INPUT_MANAGER->Init();
	RENDERER->Init();
	EVENT_MANAGER->Init();
	SCRIPT_MANAGER->Init();
	SHAPE_GENERATOR->Init();
	LEVEL_EDITOR->Init();
	FRAMERATE_MANAGER->SetMaxFrameRate(60);
	LOCALIZATION->Init();
	RNG->Init();
	CAMERA_MANAGER->Init();
	RAYCAST->Init();
	SCENE_MANAGER->Init();
	Mouse::GetInstance()->Init();

	SCENE_MANAGER->SetCurrentLevel(mIntLevel);
	SCENE_MANAGER->InitSceneLoad();
	PHYSICS_MANAGER->Init();
	ImGuiInit();
#ifdef PROTOTYPE
	ENEMY_STATE_MANAGER->Init();
	RAYCAST->Init();
#endif


	PauseEvent* start = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
	start->mPause = isEditor;
	EVENT_MANAGER->BroadcastEvent(start);

}

void Engine::HandleEvents()
{
	Uint32 flags = SDL_GetWindowFlags(pWindow);

	if (INPUT_MANAGER->IsTriggered(SDL_SCANCODE_F11)) {
		if (mFullScreen) {
			mFullScreen = false;
			FlipScreenMode();
		}
		else {
			mFullScreen = true;
			FlipScreenMode();
		}
	}

	SDL_Event e;
	if ((flags & SDL_WINDOW_MINIMIZED) | !(flags & SDL_WINDOW_INPUT_FOCUS)) {
		std::string current = SCENE_MANAGER->GetCurrent();
		if (current != "MainMenu" &&
			current != "Options" &&
			current != "LevelSelection" &&
			current != "Quit" &&
			current != "PauseMenu") {
			PauseEvent* pPauseEvent = static_cast<PauseEvent*>(MEMORY_MANAGER->GetEventFromMemory("PauseEvent"));
			pPauseEvent->mPause = true;
			EVENT_MANAGER->BroadcastEvent(pPauseEvent);
			SCENE_MANAGER->PushScene("PauseMenu");
			OBJECT_FACTORY->LoadLevel("PauseMenu");
		}

		AUDIO_MANAGER->PauseAll();

		SDL_WaitEvent(&e);
		while (e.window.event != SDL_WINDOWEVENT_SHOWN &&
			e.window.event != SDL_WINDOWEVENT_EXPOSED &&
			e.window.event != SDL_WINDOWEVENT_FOCUS_GAINED &&
			e.window.event != SDL_WINDOWEVENT_MAXIMIZED &&
			e.type != SDL_QUIT) {
			SDL_WaitEvent(&e);
		}

		if (e.type == SDL_QUIT) {
			StopEngine();
		}
		else {
			AUDIO_MANAGER->UnPauseAll();
		}

		SDL_RestoreWindow(pWindow);
	}

	//TODO: Controller, Mouse and Window Events
	while (SDL_PollEvent(&e))
	{
		ImGui_ImplSDL2_ProcessEvent(&e);
		INPUT_MANAGER->ProcessEvent(&e);
		if (e.type == SDL_QUIT)
		{
			StopEngine();
		}
	}
}



