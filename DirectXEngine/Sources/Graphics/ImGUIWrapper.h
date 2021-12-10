#pragma once

#include <windows.h>
#include "GraphicsState.h"

#ifdef ENABLE_IMGUI
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

#ifdef ENABLE_IMGUI
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

enum class IMGUIFN {
	DRAGFLOAT,
	DRAGFLOAT3,
	TEXT,
};

class ImGUIW {

public:
	struct window {
		window(const char* title);
		template<IMGUIFN fn, typename ... PARAMS>
		constexpr window& attach(PARAMS ...params) {
			if constexpr (fn == IMGUIFN::DRAGFLOAT) {
				ImGui::DragFloat(params...);
			} 
			else if constexpr (fn == IMGUIFN::DRAGFLOAT3) {
				ImGui::DragFloat3(params...);
			}
			else if constexpr (fn == IMGUIFN::TEXT) {
				ImGui::Text(params...);
			}
			else {
				static_assert("MISSED IMGUI FUNCTION" && false);
			}

			return *this;
		}
		void end();
	};

public:
	bool handleMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	void startFrame();
	void endFrame();
	window newWindow(const char* title);

private:
	friend class ImGUIWInstance;
	bool initialized = false;
	bool frameStarted = false;
	void doAssertion();
	void initialize(HWND hwnd, const GraphicsState& state);
	void destroy();
	ImGUIW() = default;
	~ImGUIW() = default;
};


class ImGUIWInstance {

private:
	static ImGUIW imgui;
	static bool imguiInitialized;

public:
	ImGUIWInstance() = default;
	~ImGUIWInstance();
	static ImGUIW* getPInstance();
	static ImGUIW* getPInstance(HWND hwnd, const GraphicsState& state);
};