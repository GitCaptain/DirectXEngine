#include <Windows.h>
#include "Engine.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "DirectxTK.lib")

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {

	Engine engine;
	engine.initialize(hInstance, "Title", "Window", 800, 600);
	while (engine.processMessages() == true) {
		engine.Update();
	}
	return 0; 
}