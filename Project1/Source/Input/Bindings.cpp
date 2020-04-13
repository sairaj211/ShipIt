#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Bindings.h"
#include "Managers/ResourceManager.h"
#include "Resources/JsonData.h"
Bindings* Bindings::mInstance = nullptr;

std::list<std::string> Bindings::GetBindings(std::string aBinding, std::string aType)
{
	if (aType == "keyboard")
		return mKeyboardBindings[aBinding];
	if (aType == "gamepad")
		return mGamePadBindings[aBinding];
	return std::list<std::string>();
}

Bindings::Bindings()
{
	mKeyboardBindings = std::unordered_map<std::string, std::list<std::string>>();
	mGamePadBindings= std::unordered_map<std::string, std::list<std::string>>();
}

Bindings::~Bindings()
{
}

std::vector<SDL_Keycode> Bindings::BindingToSDLScanCode(std::string _key)
{
	std::vector<SDL_Keycode> keys;
	auto list = mKeyboardBindings[_key];
	for (auto item : list) {
		SDL_Scancode code = SDL_GetScancodeFromName(item.c_str());
		if (SDL_SCANCODE_UNKNOWN != code) {
			keys.push_back(code);
		}
	}
	return keys;
}

std::vector<SDL_GameControllerButton> Bindings::BindingToSDLButtonCode(std::string aKey)
{
	std::vector<SDL_GameControllerButton> buttons;
	auto list = mGamePadBindings[aKey];
	for (auto item : list) {
		SDL_GameControllerButton code = SDL_GameControllerGetButtonFromString(item.c_str());
		if (SDL_CONTROLLER_BUTTON_INVALID != code) {
			buttons.push_back(code);
		}
	}
	return buttons;
}

std::vector<SDL_GameControllerAxis> Bindings::BindingToSDLAxisCode(std::string aKey)
{
	std::vector<SDL_GameControllerAxis> axises;
	auto list = mGamePadBindings[aKey];
	for (auto item : list) {
		std::vector<std::string> tokens;
		std::istringstream f(item); std::string s;
		while (getline(f, s, '|')) { tokens.push_back(s); }
		SDL_GameControllerAxis code = SDL_GameControllerGetAxisFromString(tokens[0].c_str());
		if (SDL_CONTROLLER_AXIS_INVALID != code) {
			axises.push_back(code);
		}
	}
	return axises;
}

Bindings * Bindings::GetInstance()
{
	if (mInstance == nullptr) {
		mInstance = new Bindings();
	}
	return mInstance;
}

int Bindings::BindingToMouseButton(std::string aButton)
{
	if (aButton.empty()) return -1;
	if (aButton[0] != 'm') return -1;
	if (aButton[1] == '1') return 0;
	else if (aButton[1] == '2') return 1;
	else if (aButton[1] == '3') return 2;
	return -1;
}



void Bindings::Init()
{
	Json::Value rt = ResourceManager::GetInstance()->LoadJsonData("Bindings", "./Resources/KeyBindings/")->mRoot["bindings"];
	Serialize(rt);
}

void Bindings::Serialize(Json::Value root) {

	Json::Value::Members members = root.getMemberNames();
	for (auto member : members) {
		Json::Value tmpRoot = root[member];
		for (auto binding : tmpRoot.getMemberNames()) {
			Json::Value k = tmpRoot[binding];
			std::list<std::string> keys;
			for (unsigned int i = 0; i < k.size(); ++i) {
				keys.push_back(k[i].asCString());
			}
			if (member == "keyboard") {
				mKeyboardBindings[binding] = keys;
			}
			else if (member == "controller") {
				mGamePadBindings[binding] = keys;
			}
		}
	}
}

void Bindings::DeSerialize(Json::Value & root)
{
	//add later for custom keybindings
}
