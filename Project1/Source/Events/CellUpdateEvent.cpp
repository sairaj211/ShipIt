#include "pch.h"
#include "Managers/Log.h"
#include "Core.h"
#include "CellUpdateEvent.h"

CellUpdateEvent::CellUpdateEvent() : Event("CellUpdateEvent")
{
	mMatch = false;
	mCellID = -1;
	mPlayerID = -1;
}

CellUpdateEvent::~CellUpdateEvent()
{

}

void CellUpdateEvent::Reset()
{
	mMatch = false;
	mCellID = -1;
	mPlayerID = -1;

	Event::Reset();
}

CellUpdateEvent* CellUpdateEvent::Create()
{
	return new CellUpdateEvent();
}
