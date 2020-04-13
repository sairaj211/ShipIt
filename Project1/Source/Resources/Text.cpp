#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Text.h"

Text::Text(std::string a_name) : Resource(a_name)
{
	
}

Text::~Text()
{
	if (!mLoadFailed)
		mFileStream.close();
}

void Text::Load()
{
	// Don't load if resource is already loaded
	if (!mLoadFailed)
		return;

	mLoadFailed = false;

	mFileStream.open(".\\Resources\\Text\\" + mName + ".txt");
	if (!mFileStream) {
		LOG_ERROR("Failed to load Text " + mName + ".txt");
		mLoadFailed = true;
	}
}