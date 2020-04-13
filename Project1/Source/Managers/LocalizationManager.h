#pragma once
/*!
 * Localization Manager gets text
 * in game codes map to windows codes
 */
class LocalizationManager
{
public:
	/*!
	 * Either create a new instance if one does not exist
	 * or return the current instance
	 * \return the instance current initialized
	 */
	GEAR_API static LocalizationManager* GetInstance();
	/*!
	 * Checks the directory structure to ensure that there isupport for the requested language
	 * \param aLang the language code two characters long
	 * \return True if there is a directory containing the localization for that language
	 */
	GEAR_API bool IsLanguageSupported(std::string aLang);
	/*!
	 * Get the Desired Text for a given file and language.
	 * 
	 * \param aFile The file requested it will be appended to the basepath
	 * \param aLang the language requested default is empty for the current encoding
	 * \return a UTF-8 wstring of the file's text
	 */
	GEAR_API std::wstring GetText(std::string aFile, std::string aLang = "");
	/*!
	 * Initialize the Localization, default language is English.
	 * 
	 */
	GEAR_API void Init();
	/*!
	 * Clear the mappings.
	 * 
	 */
	GEAR_API void CleanUp();
	/*!
	 * Set the base path for all audio and text that is localized
	 * 
	 * \param aPath
	 */
	GEAR_API void SetBasePath(std::string aPath);
	/*!
	 * Set the current language
	 * 
	 * \param aLang 
	 */
	GEAR_API void SetCurrentLanguage(std::string aLang);
	/*!
	 * Return the base path for localization, each of the language encodings are sub directories of this path
	 * 
	 * \return the localization base path
	 */
	GEAR_API std::string GetBasePath();
	/*!
	 * Get the full path to the localized audio
	 * 
	 * \param aLang the encoding desired default is empty to current encoding
	 * \return 
	 */
	GEAR_API std::string GetAudioPath(std::string aLang = "");
	/*!
	 * 
	 * 
	 * \return the path to the font
	 */
	GEAR_API std::string GetFont();

private:

	GEAR_API LocalizationManager();
	GEAR_API ~LocalizationManager();
private:
	static LocalizationManager* mInstance;
	std::string mBasePath;
	std::string mCurrentEncoding;
	std::unordered_map<std::string, std::string> mFonts;
};

