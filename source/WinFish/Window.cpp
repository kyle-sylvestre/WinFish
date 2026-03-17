#include "WinFishApp.h"
#include <SDL2/SDL_main.h>
#include <SexyAppFramework/SexyAppBase.h>

using namespace Sexy;

int main(int argc, char *argv[])
{
	WinFishApp* aTheApp = new WinFishApp();
    
    const char *dir = "/Users/ksylvestre/dev/__WinFish/ignore";
    Sexy::SetResourceFolder(dir);
    Sexy::ChDir(dir);

	aTheApp->Init();
	aTheApp->Start();
#if !defined(SDL_PLATFORM_EMSCRIPTEN)
	aTheApp->Shutdown();
	delete aTheApp;
#endif
	return 0;
}
