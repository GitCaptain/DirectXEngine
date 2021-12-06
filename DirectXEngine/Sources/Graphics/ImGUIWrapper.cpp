#include "ImGUIWrapper.h"

void ImGUIW::doAssertion() {
    assert(initialized && "You should initilize Imgui before start using it");
}

void ImGUIW::initialize(HWND hwnd, const GraphicsState& state) {
#ifdef ENABLE_IMGUI
    // setup ImGUI
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(state.device, state.deviceContext);
    ImGui::StyleColorsDark();
#endif
    initialized = true;
}

void ImGUIW::destroy() {
#ifdef ENABLE_IMGUI
    doAssertion();
    ImGui::DestroyContext();
#endif
}

bool ImGUIW::handleMsg(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
#ifdef ENABLE_IMGUI
    // it is ok, to not assert here
    // imgui check if it has context inside
    return ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam);
#else
    return false;
#endif
}

void ImGUIW::startFrame() {
#ifdef ENABLE_IMGUI
    doAssertion();
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
#endif
}

void ImGUIW::endFrame() {
#ifdef ENABLE_IMGUI
    doAssertion();
    // Assemble Together Draw data
    ImGui::Render();
    // render draw data
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}

ImGUIW::window ImGUIW::newWindow(const char* title) {
#ifdef ENABLE_IMGUI
    doAssertion();
    return window(title);
#endif
}

ImGUIW::window::window(const char *title) {
    ImGui::Begin(title);
}

void ImGUIW::window::end() {
    ImGui::End();
}

ImGUIWInstance::~ImGUIWInstance() {
#ifdef ENABLE_IMGUI
    imgui.destroy();
#endif
}

ImGUIW* ImGUIWInstance::getPInstance() {
#ifdef ENABLE_IMGUI
    return &imgui;
#else
    return nullptr;
#endif
}

ImGUIW* ImGUIWInstance::getPInstance(HWND hwnd, const GraphicsState& state) {
#ifdef ENABLE_IMGUI
    assert(!imguiInitialized && "ImGui already initialized. Use same function without parameters to get instance.");
    imgui.initialize(hwnd, state);
    imguiInitialized = true;
    return &imgui;
#else
    return nullptr;
#endif
}

ImGUIW ImGUIWInstance::imgui;
bool ImGUIWInstance::imguiInitialized = false;
