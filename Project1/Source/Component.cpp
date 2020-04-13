#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "Component.h"

Component::Component(std::string a_type)
{
	mType = a_type;
	mPaused = false;
}

Component::~Component()
{

}

void Component::Reset()
{
//	mType = "Empty";
	mOwnerID = 0;
	mPaused = false;
}

void Component::SetType(std::string a_type)
{
	mType = a_type;
}

std::string Component::GetType()
{
	return mType;
}

void Component::SetOwnerID(int a_ownerID)
{
	mOwnerID = a_ownerID;

	// TODO: Change owner id in the entity-component map?
}

int Component::GetOwnerID()
{
	return mOwnerID;
}

bool Component::IsPaused()
{
	return mPaused;
}

void Component::SetDirty(int a_dirtyBits)
{
	mDirty |= a_dirtyBits;
}

bool Component::IsDirty(int a_dirtyBits)
{
	return (mDirty & a_dirtyBits) == a_dirtyBits;
}
