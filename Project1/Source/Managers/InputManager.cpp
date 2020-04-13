#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "InputManager.h"
#include "Input/GamePad.h"
#include "Engine.h"
#include "Input/Bindings.h"

InputManager* InputManager::mInstance = nullptr;

#define BINDINGS Bindings::GetInstance()


InputManager::~InputManager()
{
	for (auto kv : mGamePads) {
		delete kv.second;
	}
	mGamePads.clear();
	mMouse->CleanUp();
}

InputManager* InputManager::GetInstance()
{
	if (!mInstance) {
		mInstance = new InputManager();
	}
	return mInstance;
}

void InputManager::Init()
{
	LOG_INFO("InputManager Init");

	SDL_memset(mCurrentState, 0, 512 * sizeof(Uint8));
	SDL_memset(mPreviousState, 0, 512 * sizeof(Uint8));
	SDL_memset(mKeyTime, 0, 512 * sizeof(Uint32));
	BINDINGS->Init();
	mGamePads = std::unordered_map<int, GamePad*>();
	for (int i = PLAYER_1; i < (int)MAX_PLAYERS; ++i) {
		mGamePads.emplace(i, new GamePad());
		mGamePads[i]->SetController(SDL_GameControllerOpen(i));
	}
	mMouse = Mouse::GetInstance();
	mMouse->Init();

}

void InputManager::Update()
{
	//Update the gamepads as required
	int active_count = 0;
	for (auto kv : mGamePads)
	{
		if (kv.second->IsActive()) 
			++active_count;
	}
	int num_sticks = SDL_NumJoysticks();
	if (num_sticks != active_count) {
		int control_count = 0;
		for (int key = 0; key < MAX_PLAYERS; ++key)
		{
			if (!mGamePads[key]->IsActive()) {
				mGamePads[key]->SetController(SDL_GameControllerOpen(control_count));
			}
			++control_count;
		}
	}
	for (auto kv : mGamePads)
	{
		if (kv.second->IsActive()) {
			kv.second->Update();
		}
	}

	

	//SDL_memcpy(mPrevMouseState, mCurrentMouseState, 3 * sizeof(bool));
	
	// Store Previous Mouse State
	mMouse->Update();
	
	int numberOfFetchedKeys = 0;
    const Uint8* pCurrentKeyStates = SDL_GetKeyboardState(&numberOfFetchedKeys);

	if (numberOfFetchedKeys > 512)
	{
		numberOfFetchedKeys = 512;
	}
	SDL_memcpy(mPreviousState, mCurrentState, 512 * sizeof(Uint8));
	SDL_memcpy(mCurrentState, pCurrentKeyStates, numberOfFetchedKeys * sizeof(Uint8));
	//update controllers
}

void InputManager::ProcessEvent(SDL_Event * event)
{
	mMouse->ProcessMouseEvents(event);
}

GEAR_API std::list<int> InputManager::GetPlayerGamePads()
{
	std::list<int> result;
	for (auto kv : mGamePads)
	{
		if (kv.second->IsActive())
			result.push_back(kv.first);
	}
	return result;
}


void InputManager::Cleanup()
{
	delete mInstance;
}

bool InputManager::IsPressed(unsigned int KeyScanCode)
{
	if (KeyScanCode >= 512)
		return false;

	if (mCurrentState[KeyScanCode])
	{
		mKeyTime[KeyScanCode]++;
		return true;
	}

	return false;
}

bool InputManager::IsTriggered(unsigned int KeyScanCode)
{

	if (KeyScanCode >= 512)
		return false;

	if (mCurrentState[KeyScanCode] && !mPreviousState[KeyScanCode])
	{
		mKeyTime[KeyScanCode] = 0;
		return true;
	}

	return false;
}

bool InputManager::IsReleased(unsigned int KeyScanCode)
{
	if (KeyScanCode >= 512)
		return false;

	if (!mCurrentState[KeyScanCode] && mPreviousState[KeyScanCode])
	{
		mKeyTime[KeyScanCode] = 0;
		return true;
	}
	return false;
}

bool InputManager::IsPressed(std::string pressed, int _playerNum)
{
	int mouse = BINDINGS->BindingToMouseButton(BINDINGS->GetBindings(pressed, "keyboard").front());
	if (mouse >= 0 && mouse < 3)
		return mMouse->IsPressed(mouse);

	for (int i = 0; i < mGamePads.size(); ++i) {
		GamePad* pad = mGamePads[i];
		if (pad->IsActive() && ((_playerNum >= MAX_PLAYERS || _playerNum < 0) || (_playerNum == i))) {
			std::list<std::string> bindings = BINDINGS->GetBindings(pressed, "gamepad");
			for (auto binding : bindings) {
				std::vector<std::string> tokens;
				std::istringstream f(binding); std::string s;///get binding value instead to use
				while (getline(f, s, '|')) { tokens.push_back(s); }

				std::vector<SDL_GameControllerAxis> axis = BINDINGS->BindingToSDLAxisCode(pressed);
				bool axis_pressed = false;
				//test the axis first
				for (auto a : axis) {

					axis_pressed = (pad->IsStickPressedDown(a) && tokens[1] == "down") || (pad->IsStickPressedUp(a) && tokens[1] == "up") ||
						(pad->IsStickPressedLeft(a) && tokens[1] == "left") || (pad->IsStickPressedRight(a) && tokens[1] == "right");

					if (axis_pressed) return true;
				}
			}
			//check buttons
			std::vector<SDL_GameControllerButton> buttons = BINDINGS->BindingToSDLButtonCode(pressed);
			bool button_pressed = false;
			for (auto b : buttons) {
				button_pressed = pad->IsPressed(b);
				if (button_pressed) return true;
			}
		}
	}
	
	std::vector<SDL_Keycode> keys = BINDINGS->BindingToSDLScanCode(pressed);
	bool key_pressed = false;
	for (auto k : keys) {
		key_pressed = IsPressed(k);
		if (key_pressed) return true;
	}
	
	return false;
}

bool InputManager::IsTriggered(std::string trigger, int _playerNum)
{
	int mouse = BINDINGS->BindingToMouseButton(BINDINGS->GetBindings(trigger, "keyboard").front());
	if (mouse >= 0 && mouse < 3)
		return mMouse->IsTriggered(mouse);

	for (int i = 0; i < mGamePads.size(); ++i) {
		GamePad* pad = mGamePads[i];
		if (pad->IsActive() && ((_playerNum >= MAX_PLAYERS || _playerNum < 0) || (_playerNum == i))) {
			std::list<std::string> bindings = BINDINGS->GetBindings(trigger, "gamepad");
			for (auto binding : bindings) {
				std::vector<std::string> tokens;
				std::istringstream f(binding); std::string s;///get binding value instead to use
				while (getline(f, s, '|')) { tokens.push_back(s); }

				std::vector<SDL_GameControllerAxis> axis = BINDINGS->BindingToSDLAxisCode(trigger);
				bool axis_pressed = false;
				//test the axis first
				for (auto a : axis) {

					axis_pressed = (pad->IsStickTriggeredDown(a) && tokens[1] == "down") || (pad->IsStickTriggeredUp(a) && tokens[1] == "up") ||
						(pad->IsStickTriggeredLeft(a) && tokens[1] == "left") || (pad->IsStickTriggeredRight(a) && tokens[1] == "right");

					if (axis_pressed) return true;
				}
				//check buttons
			}
			std::vector<SDL_GameControllerButton> buttons = BINDINGS->BindingToSDLButtonCode(trigger);
			bool button_pressed = false;
			for (auto b : buttons) {
				button_pressed = pad->IsTriggered(b);
				if (button_pressed) return true;
			}
		}
	}

	std::vector<SDL_Keycode> keys = BINDINGS->BindingToSDLScanCode(trigger);
	bool key_pressed = false;
	for (auto k : keys) {
		key_pressed = IsTriggered(k);
		if (key_pressed) return true;
	}

	return false;
}

bool InputManager::IsReleased(std::string released, int _playerNum)
{
	int mouse = BINDINGS->BindingToMouseButton(BINDINGS->GetBindings(released, "keyboard").front());
	if (mouse >= 0 && mouse < 3)
		return mMouse->IsReleased(mouse);

	for (int i = 0; i < mGamePads.size(); ++i) {
		GamePad* pad = mGamePads[i];
		if (pad->IsActive() && ((_playerNum >= MAX_PLAYERS || _playerNum < 0) || (_playerNum == i))) {
			std::list<std::string> bindings = BINDINGS->GetBindings(released, "gamepad");
			for (auto binding : bindings) {
				std::vector<std::string> tokens;
				std::istringstream f(binding); std::string s;///get binding value instead to use
				while (getline(f, s, '|')) { tokens.push_back(s); }

				std::vector<SDL_GameControllerAxis> axis = BINDINGS->BindingToSDLAxisCode(released);
				bool axis_pressed = false;
				//test the axis first
				for (auto a : axis) {

					axis_pressed = (pad->IsStickReleasedDown(a) && tokens[1] == "down") || (pad->IsStickReleasedUp(a) && tokens[1] == "up") ||
						(pad->IsStickReleasedLeft(a) && tokens[1] == "left") || (pad->IsStickReleasedRight(a) && tokens[1] == "right");

					if (axis_pressed) return true;
				}
			}
			//check buttons
			std::vector<SDL_GameControllerButton> buttons = BINDINGS->BindingToSDLButtonCode(released);
			bool button_pressed = false;
			for (auto b : buttons) {
				button_pressed = pad->IsReleased(b);
				if (button_pressed) return true;
			}
		}
	}
	
	std::vector<SDL_Keycode> keys = BINDINGS->BindingToSDLScanCode(released);
	bool key_pressed = false;
	for (auto k : keys) {
		key_pressed = IsReleased(k);
		if (key_pressed) return true;
	}
	
	return false;
}

Uint32 InputManager::GetTime(unsigned int KeyScanCode)
{
	return mKeyTime[KeyScanCode];
}

GEAR_API void InputManager::ConfirmGamepad(int aId, bool aConfirm)
{
	mCofirmed.emplace(aId, aConfirm);
}
