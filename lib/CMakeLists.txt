add_library(ImGui imgui/imgui.cpp imgui/imgui_draw.cpp imgui/imgui_widgets.cpp)
add_library(ImGuiDemo imgui/imgui_demo.cpp)
add_library(ImGuiOpenGL3 imgui/examples/imgui_impl_glfw.cpp imgui/examples/imgui_impl_opengl3.cpp)
add_library(gl3w imgui/examples/libs/gl3w/GL/gl3w.c)

add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GL3W)

#set(CMAKE_CXX_FLAGS "-g")
