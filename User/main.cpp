#if defined(DEBUG) | defined(_DEBUG)
	#pragma comment(linker, "/entry:wWinMainCRTStartup /subsystem:console")
#endif

#include "../Core/Core.h"
#include "MainScene.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	auto core = std::make_unique<Core>(new MainScene);

	return 0;
}