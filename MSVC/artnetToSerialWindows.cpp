// ImGui - standalone example application for DirectX 11
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.



#include "../SerialInterface.h"
#include "../gui/SerialWindow.h"
#include "../LoggingUtilities.h"
#include "../gui/LogWindow.h"
#include "../ArtnetWindow.h"
#include "../ArtnetThread.h"
#include "../DMXBucket.h"
#include "../gui/DMXWindow.h"
#include "../SerialThread.h"
#include <cstdio>
#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/thread.hpp>
//#include <boost/log/attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>

#include <imgui.h>

#ifdef _WIN32
#include "../platform/WindowsGraphics.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#endif

int main(int, char**)
{
	boost::log::add_common_attributes();
	auto console_sink = boost::log::add_console_log(std::cout);
	console_sink->set_formatter(&LoggingUtilities::coloring_terminal_formatter);

	boost::log::core::get()->set_filter
		(
			boost::log::trivial::severity >= boost::log::trivial::debug
			);
	BOOST_LOG_TRIVIAL(info) << "Starting artnetToSerial...";

#ifdef _WIN32
	// Create application window
	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGui Example"), NULL };
	::RegisterClassEx(&wc);
	HWND hwnd = CreateWindow(_T("ImGui Example"), _T("Artnet To Serial"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

	// Initialize Direct3D
	if (CreateDeviceD3D(hwnd) < 0)
	{
		CleanupDeviceD3D();
		UnregisterClass(_T("ImGui Example"), wc.hInstance);
		return 1;
	}

	// Show the window
	ShowWindow(hwnd, SW_SHOWDEFAULT);
	UpdateWindow(hwnd);

	// Initialize ImGui
	ImGui::CreateContext();

	// Setup ImGui binding
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
#endif

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'extra_fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//ImGuiIO& io = ImGui::GetIO();
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../extra_fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	bool show_test_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Initialise the logging window
	auto logWindow_p = std::make_shared<LogWindow>();
	LogWindow &logWindow = *logWindow_p;
	typedef boost::log::sinks::synchronous_sink<LoggingUtilities::GUISinkBackend> sink_t;
	//std::shared_ptr<sink_t> sink(new sink_t(logWindow_p));
	//boost::log::core::get()->add_sink(sink);

	// Initialise the DMX Bucket
	auto dmxBucket_p = std::make_shared<DMXBucket>();
	DMXBucket &dmxBucket = *dmxBucket_p;

	// Initialise the serial interface
	auto serialInterface_p = std::make_shared<SerialInterface>();

	// Initialise the serial and artnet windows
	SerialWindow serialWindow(serialInterface_p);
	auto artnetWindow_p = std::make_shared<ArtnetWindow>();
	ArtnetWindow &artnetWindow = *artnetWindow_p;
	DMXWindow dmxWindow(dmxBucket);

	// Initialise the serial and artnet threads
	auto serialUpdater = std::make_shared<SerialThread::Updater>();
	SerialThread serialThread(serialInterface_p, dmxBucket_p, serialUpdater);
	ArtnetThread artnetThread(artnetWindow_p, dmxBucket_p, serialUpdater);
	boost::thread t_1(artnetThread);
	boost::thread t_2(serialThread);

	// Pass some dependency injection
	dmxWindow.setSerialUpdater(serialUpdater);

	// Main loop
#ifdef _WIN32
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));
	while (msg.message != WM_QUIT)
	{
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
			continue;
		}
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
#endif
		ImGui::NewFrame();

		// 1. Show a simple window.
		// Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug".
		{
			static float f = 0.0f;
			ImGui::Text("Hello, world!");
			ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
			ImGui::ColorEdit3("clear color", (float*)&clear_color);
			if (ImGui::Button("Test Window")) show_test_window ^= 1;
			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		}

		// 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow().
		if (show_test_window)
		{
			ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);     // Normally user code doesn't need/want to call it because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
			ImGui::ShowDemoWindow(&show_test_window);
		}

		serialWindow.draw();
		logWindow.draw();
		artnetWindow.draw();
		dmxWindow.draw();

		// Rendering
		ImGui::Render();
#ifdef _WIN32
		g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
		g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0); // Present with vsync
		//g_pSwapChain->Present(0, 0); // Present without vsync
	}

	ImGui_ImplDX11_Shutdown();
	CleanupDeviceD3D();
	UnregisterClass(_T("ImGui Example"), wc.hInstance);
#endif

	return 0;
}
