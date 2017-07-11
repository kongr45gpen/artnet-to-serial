// ImGui - standalone example application for Glfw + OpenGL 2, using fixed pipeline
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "SerialInterface.h"
#include "gui/SerialWindow.h"
#include "LoggingUtilities.h"
#include "gui/LogWindow.h"
#include "ArtnetWindow.h"
#include "ArtnetThread.h"
#include "DMXBucket.h"
#include "gui/DMXWindow.h"
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
#include <GLFW/glfw3.h>

int main(int, char **) {
//    boost::log::add_file_log("sample.log");
    boost::log::add_common_attributes();
    auto console_sink = boost::log::add_console_log(std::cout);
    console_sink->set_formatter(&LoggingUtilities::coloring_terminal_formatter);

    boost::log::core::get()->set_filter
            (
                    boost::log::trivial::severity >= boost::log::trivial::debug
            );
    BOOST_LOG_TRIVIAL(info) << "Starting artnetToSerial...";
//    boost::log::core::get()->set_logging_enabled(false);

    // Setup window
    glfwSetErrorCallback([](int error, const char *description) {
        BOOST_LOG_TRIVIAL(error) << "GLFW Error: " << error << description;
    });
    if (!glfwInit())
        return 1;
    GLFWwindow *window = glfwCreateWindow(1280, 720, "Artnet to Serial", NULL, NULL);
    glfwMakeContextCurrent(window);

    // Setup ImGui binding
    ImGui_ImplGlfw_Init(window, true);

    bool show_test_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);

    auto logWindow_p = std::make_shared<LogWindow>();
    LogWindow &logWindow = *logWindow_p;
    typedef boost::log::sinks::synchronous_sink<LoggingUtilities::GUISinkBackend> sink_t;
    boost::shared_ptr<sink_t> sink(new sink_t(logWindow_p));
    boost::log::core::get()->add_sink(sink);

    auto dmxBucket_p = std::make_shared<DMXBucket>();
    DMXBucket &dmxBucket = *dmxBucket_p;

    SerialWindow serialWindow;
    auto artnetWindow_p = std::make_shared<ArtnetWindow>();
    ArtnetWindow &artnetWindow = *artnetWindow_p;
    DMXWindow dmxWindow(dmxBucket);

    ArtnetThread artnetThread(artnetWindow_p, dmxBucket_p);
    boost::thread t(artnetThread);

    // Main loop
    while (!glfwWindowShouldClose(window)) {
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED)) {
            // Don't waste CPU rendering when the window is minimised
            glfwWaitEvents();
            continue;
        }
        glfwPollEvents();
        ImGui_ImplGlfw_NewFrame();

        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float *) &clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate,
                        ImGui::GetIO().Framerate);
        }

        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window) {
            ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }

        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window) {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }

        serialWindow.draw();
        logWindow.draw();
        artnetWindow.draw();
        dmxWindow.draw();

        // Rendering
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui::Render();
        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplGlfw_Shutdown();
    glfwTerminate();

    return 0;
}
