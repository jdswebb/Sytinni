#include "imgui_implementation.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_impl_dx9.h"
#include <vector>

#pragma comment(lib, "imgui/lib/imgui.lib")


namespace imgui_implementation
{

static std::vector<void(*)()> renderCallbacks;

bool enableImgui;

void enableInternalUi(bool enable)
{
	 enableImgui = enable;
}

WNDPROC originalWndProcHandler = nullptr;
IMGUI_API LRESULT hkWndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	 ImGuiIO& io = ImGui::GetIO();
	 switch (msg)
	 {
	 case WM_LBUTTONDOWN:
		  io.MouseDown[0] = true;
		  break;
	 case WM_LBUTTONUP:
		  io.MouseDown[0] = false;
		  break;
	 case WM_RBUTTONDOWN:
		  io.MouseDown[1] = true;
		  break;
	 case WM_RBUTTONUP:
		  io.MouseDown[1] = false;
		  break;
	 case WM_MBUTTONDOWN:
		  io.MouseDown[2] = true;
		  break;
	 case WM_MBUTTONUP:
		  io.MouseDown[2] = false;
		  break;
	 case WM_MOUSEWHEEL:
		  io.MouseWheel += GET_WHEEL_DELTA_WPARAM(wParam) > 0 ? +1.0f : -1.0f;
		  break;
	 case WM_MOUSEMOVE:
		  io.MousePos.x = (signed short)(lParam);
		  io.MousePos.y = (signed short)(lParam >> 16);
		  break;
	 case WM_KEYDOWN:
		  if (wParam < 256)
				io.KeysDown[wParam] = 1;
		  break;
	 case WM_KEYUP:
		  if (wParam < 256)
				io.KeysDown[wParam] = 0;
		  break;
	 case WM_CHAR:
		  // You can also use ToAscii()+GetKeyboardState() to retrieve characters.
		  if (wParam > 0 && wParam < 0x10000)
				io.AddInputCharacter((unsigned short)wParam);
		  break;
	 }

	 return CallWindowProc(originalWndProcHandler, hwnd, msg, wParam, lParam);
}

bool isSetup = false;
 void setup(IDirect3DDevice9* pDevice)
 {
	  if (isSetup)
			return;

	  D3DDEVICE_CREATION_PARAMETERS cParam;

	  pDevice->GetCreationParameters(&cParam);

	  IMGUI_CHECKVERSION();
	  ImGui::CreateContext();
	  ImGui_ImplWin32_Init(cParam.hFocusWindow);
	  ImGui_ImplDX9_Init(pDevice);

	  ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;

	  ImGui::GetIO().WantCaptureKeyboard = true;
	  ImGui::GetIO().WantCaptureMouse = true;
	  ImGui::GetIO().WantTextInput = true;
	  originalWndProcHandler = (WNDPROC)SetWindowLongPtr(cParam.hFocusWindow, GWL_WNDPROC, (LONG)hkWndProcHandler);

	  ImGuiStyle& style = ImGui::GetStyle();
	  style.Colors[ImGuiCol_Text] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f); // ImVec4(0.78f, 0.78f, 0.78f, 1.00f);
	  style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.59f, 0.59f, 0.59f, 1.00f);
	  style.Colors[ImGuiCol_WindowBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	  style.Colors[ImGuiCol_ChildBg] = ImVec4(0.11f, 0.11f, 0.11f, 1.00f);
	  style.Colors[ImGuiCol_Border] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	  style.Colors[ImGuiCol_FrameBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f);
	  style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	  style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f);
	  style.Colors[ImGuiCol_TitleBg] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	  style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.20f);
	  style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.00f, 0.48f, 0.80f, 1.00f); //ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	  style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
	  style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	  style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f); //ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	  style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f); //ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	  style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	  style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.59f);
	  style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f); //ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	  style.Colors[ImGuiCol_Button] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f); //ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	  style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f); //ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	  style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.04f, 0.04f, 0.04f, 0.88f); //ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
	  style.Colors[ImGuiCol_Header] = ImVec4(0.00f, 0.48f, 0.80f, 1.00f); //ImVec4(0.24f, 0.40f, 0.95f, 1.00f);
	  style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.80f, 0.80f, 0.80f, 0.09f); //ImVec4(0.24f, 0.40f, 0.95f, 0.59f);
	  style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);

	  style.WindowRounding = 0.f;
	  style.FramePadding = ImVec2(4, 1);
	  style.ScrollbarSize = 10.f;
	  style.ScrollbarRounding = 0.f;
	  style.GrabMinSize = 5.f;

	  isSetup = true;
 }

 void render()
 {
	  if (isSetup && enableImgui)
	  {
			ImGui_ImplDX9_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Tests", 0, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse); // ImVec2(250, 300), 0.9f,  ImGuiWindowFlags_NoResize |
			{
				 if (ImGui::Button("Test"))
				 {

				 }
			}

			for (const auto& func : renderCallbacks) // ToDo add an additional callback to host controls in the future main ImGui window
			{
				 func();
			}

			ImGui::End();

			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	  }
 }

void addRenderCallback(void(*func)())
 {
	  renderCallbacks.emplace_back(func);
 }

}
