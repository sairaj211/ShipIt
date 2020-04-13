#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Resource.h"

Resource::Resource(std::string a_name)
{
	mName = a_name;
	mLoadFailed = true;
}

Resource::~Resource()
{

}

void Resource::Reset()
{
	mLoadFailed = true;
}

bool Resource::LoadFailed()
{
	return mLoadFailed;
}