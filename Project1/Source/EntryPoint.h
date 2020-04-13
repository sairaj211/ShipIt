#pragma once

#ifdef GEAR_PLATFORM_WINDOWS

extern Engine* CreateApplication();
#undef main
int main(int agrc, char** argv)
{
	auto app = CreateApplication();
	app->Run();
	app->Close();
	delete app;
	return 0;
}

#endif