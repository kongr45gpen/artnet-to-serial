// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "SerialInterface.h"
#include "gui/SerialWindow.h"
#include "LoggingUtilities.h"
#include "gui/LogWindow.h"
#include "gui/ArtnetWindow.h"
#include "ArtnetThread.h"
#include "DMXBucket.h"
#include "gui/DMXWindow.h"
#include "SerialThread.h"
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

#ifdef _WIN32
#include "../platform/WindowsGraphics.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#else
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GL/gl3w.h>
#include <GLFW/glfw3.h>
#endif

int main(int, char **) {
//    boost::log::add_file_log("sample.log");
    boost::log::add_common_attributes();
    auto console_sink = boost::log::add_console_log(std::cout);
    console_sink->set_formatter(&LoggingUtilities::coloring_terminal_formatter);

    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::debug
    );
    BOOST_LOG_TRIVIAL(info) << "Starting artnetToSerial...";
//    boost::log::core::get()->set_logging_enabled(false);

#ifdef _WIN32
    // Create application window
    WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("artlight ImGui"), NULL };
    ::RegisterClassEx(&wc);
    HWND hwnd = CreateWindow(_T("artlight ImGui"), _T("Artnet To Serial"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);

    // Initialize Direct3D
    if (CreateDeviceD3D(hwnd) != 1)
    {
        CleanupDeviceD3D();
        UnregisterClass(_T("artlight ImGui"), wc.hInstance);
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
#else
    // Decide GL+GLSL versions
#if __APPLE__
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Initialize ImGui
    ImGui::CreateContext();

    // Setup window
    if (!glfwInit()) {
        BOOST_LOG_TRIVIAL(fatal) << "Could not init glfw";
        return 1;
    }
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Artnet to Serial", NULL, NULL);
    if (window == nullptr) {
        BOOST_LOG_TRIVIAL(fatal) << "Could not create window";
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (gl3wInit() != 0 ){
        BOOST_LOG_TRIVIAL(fatal) << "Failed to initialize OpenGL loader!";
        return 1;
    }

    // Setup ImGui binding
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif

    bool show_test_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    // Initialise the logging window
    auto logWindow_p = std::make_shared<LogWindow>();
    LogWindow &logWindow = *logWindow_p;
    typedef boost::log::sinks::synchronous_sink<LoggingUtilities::GUISinkBackend> sink_t;
    auto guiSinkBackend = boost::make_shared<LoggingUtilities::GUISinkBackend>(logWindow_p);
    boost::shared_ptr<sink_t> sink(new sink_t(guiSinkBackend));
    boost::log::core::get()->add_sink(sink);

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
        ImGui::NewFrame();
#else
    while (!glfwWindowShouldClose(window)) {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
#endif

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("artlight");
            ImGui::Text("Artnet To Serial");
            ImGui::ColorEdit3("clear color", (float *) &clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
            ImGui::ShowDemoWindow(&show_test_window);
        }

        serialWindow.draw();
        logWindow.draw();
        artnetWindow.draw();
        dmxWindow.draw();

        // Rendering
#ifdef _WIN32
        ImGui::Render();

        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, (float*)&clear_color);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        g_pSwapChain->Present(1, 0); // Present with vsync
        //g_pSwapChain->Present(0, 0); // Present without vsync
#else
        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
#endif
    }

    // Cleanup
#ifdef _WIN32
    ImGui_ImplDX11_Shutdown();
    CleanupDeviceD3D();
    UnregisterClass(_T("artlight ImGui"), wc.hInstance);
#else
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
#endif

    return 0;
}
