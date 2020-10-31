#include <Windows.h>
#include "Engine.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR lpCmdLine,
	_In_ int nCmdShow) {

	HRESULT hr = CoInitialize(nullptr);
	ONFAILHRLOG(hr, "Failed to call CoInitialize")

	Engine engine;
	if (engine.initialize(hInstance, "Engine", "Window", 800, 600)) {
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