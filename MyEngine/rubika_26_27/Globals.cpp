#include "Globals.h"

#include <DebugMgr.h>
#include <Logger.h>

Globals gData;

Globals::Globals() : FrameCount(0)
{
	SceneMgr = new ::SceneMgr();
	DebugMgr = new ::DebugMgr();
	Logger = new ::Logger();
}

Globals::~Globals()
{
	Destroy();
}

void Globals::Init()
{
	SceneMgr->Init();
	Logger->Init();
}

void Globals::Shut()
{
	SceneMgr->Shut();
	Logger->Shut();
}

void Globals::Destroy()
{
	delete SceneMgr;
	SceneMgr = nullptr;
	
	delete DebugMgr;
	DebugMgr = nullptr;

	delete Logger;
	Logger = nullptr;
}

void Globals::IncrementFrameCount()
{
	++FrameCount;
}
