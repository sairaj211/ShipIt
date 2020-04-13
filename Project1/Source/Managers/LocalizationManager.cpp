#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "LocalizationManager.h"
#include "Renderer.h"
#include <codecvt>

LocalizationManager* LocalizationManager::mInstance = nullptr;

using namespace std::filesystem;

LocalizationManager * LocalizationManager::GetInstance()
{
	if (mInstance == nullptr) {
		mInstance = new LocalizationManager();
	}
	return mInstance;
}

bool LocalizationManager::IsLanguageSupported(std::string aLang)
{
	return exists(path(mBasePath+aLang));
}

std::wstring LocalizationManager::GetText(std::string aFile, std::string aLang)
{
	std::string fullPath;
	std::string encode = mCurrentEncoding;
	if (!aLang.empty())
		encode = aLang;
	if (IsLanguageSupported(encode))
		fullPath = mBasePath + encode + "/" + aFile;
	else
		return std::wstring();

	std::wifstream stream(fullPath);
	stream.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
	std::wstringstream buffer;
	buffer << stream.rdbuf();
	return buffer.str();
}

void LocalizationManager::Init()
{
	LOG_INFO("LocalizationManager Init");
	mBasePath = "./Resources/Localization/";
	mCurrentEncoding = "en"; //find language and default to english
}

void LocalizationManager::CleanUp()
{
	delete mInstance;
	mInstance = nullptr;
}

void LocalizationManager::SetBasePath(std::string aPath)
{
	mBasePath = aPath;
}

void LocalizationManager::SetCurrentLanguage(std::string aLang)
{
	if(IsLanguageSupported(aLang))
		mCurrentEncoding = aLang;
	Renderer::GetInstance()->ReloadFonts(mFonts[aLang]);
}


std::string LocalizationManager::GetBasePath()
{
	return mBasePath;
}

std::string LocalizationManager::GetFont()
{
	return mFonts[mCurrentEncoding];
}

LocalizationManager::LocalizationManager()
{
	std::locale::global(std::locale(std::locale::empty(), new std::codecvt_utf8<wchar_t>));
}


LocalizationManager::~LocalizationManager()
{
}
