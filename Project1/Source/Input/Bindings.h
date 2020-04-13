#pragma once
/*!
 * \class Class to hold all agnostic bindings and to allow for translations for SDL gamepad and key reading
 * \brief Class to hold all agnostic bindings
 * 
 */
class Bindings
{
public:
	/*!
	 *  Initialize the Bindings
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * readd all bindins from a json root
	 * 
	 * \param root to read from
	 */
	GEAR_API void Serialize(Json::Value root);
	/*!
	 * Write all of the bindings to a json root
	 * 
	 * \param root to write to
	 */
	GEAR_API void DeSerialize(Json::Value& root);
	
	/*!
	 *  Get all keys based on a binding
	 *
	 * \param _key binding string in question
	 * \return all relevent SDL keycodes, empty if none found
	 */
	GEAR_API std::vector<SDL_Keycode> BindingToSDLScanCode(std::string _key);
	/*!
	 *  Get all keys based on a binding
	 * 
	 * \param _key binding string in question
	 * \return all relevent SDL buttons, empty if none found
	 */
	GEAR_API std::vector<SDL_GameControllerButton> BindingToSDLButtonCode(std::string _key);
	/*!
	 * Get axis based on a binding
	 * 
	 * \param _axis the string for the axis
	 * \return all relevent axis empty if none found
	 */
	GEAR_API std::vector<SDL_GameControllerAxis> BindingToSDLAxisCode(std::string _axis);
	/*!
	 * Get current binding instance
	 * 
	 * \return current instance
	 */
	GEAR_API static Bindings* GetInstance();
	/*!
	 * Get the indicated mousebutton from a string
	 * 
	 * \param _button mouse button string representation
	 * \return SDL Mouse button code
	 */
	GEAR_API int BindingToMouseButton(std::string _button);

	/*!
	 * Get all of the keys relevent for a given binding
	 * 
	 * \param aType where to pull from keyboard or gamepad default keyboard
	 * \param aBinding the agnostic binding
	 * \return all relevent bindings 
	 */
	GEAR_API std::list<std::string> GetBindings(std::string aBinding, std::string aType = "keyboard");

private:
	GEAR_API Bindings();
	GEAR_API ~Bindings();
private:
	static Bindings * mInstance;
	std::unordered_map<std::string,std::list<std::string>> mKeyboardBindings;
	std::unordered_map<std::string, std::list<std::string>> mGamePadBindings;
};

