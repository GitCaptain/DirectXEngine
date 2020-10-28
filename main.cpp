#include <Windows.h>
#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {

	Engine engine;
	if (engine.initialize(hInstance, "Title", "Window", 800, 600)) {
		while (engine.processMessages()) {
			engine.Update();
			engine.renderFrame();
		}
	}
	else {
		OutputDebugStringA("Fuck\n");
	}
	return 0; 
}