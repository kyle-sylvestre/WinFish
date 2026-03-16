#include "WinFishApp.h"
#include <SDL3/SDL_main.h>

using namespace Sexy;

int main(int argc, char *argv[])
{
	WinFishApp* aTheApp = new WinFishApp();

	aTheApp->Init();
	aTheApp->Start();
#if !defined(SDL_PLATFORM_EMSCRIPTEN)
	aTheApp->Shutdown();
	delete aTheApp;
#endif
	return 0;
}
