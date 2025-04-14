add_library(imgui STATIC
    # Among the different backends available, we are interested in connecting
    # the GUI to GLFW and OpenGL:
    backends/imgui_impl_opengl3.h
    backends/imgui_impl_opengl3.cpp
    backends/imgui_impl_glfw.h
    backends/imgui_impl_glfw.cpp

    # Bonus to add some C++ specific features (the core ImGUi is a C library)
    misc/cpp/imgui_stdlib.h
    misc/cpp/imgui_stdlib.cpp

    # The core ImGui files
    imconfig.h
    imgui.h
    imgui.cpp
    imgui_demo.cpp
    imgui_draw.cpp
    imgui_internal.h
    imgui_tables.cpp
    imgui_widgets.cpp
    imstb_rectpack.h
    imstb_textedit.h
    imstb_truetype.h
)

set_target_properties(imgui PROPERTIES CXX_STANDARD 17)
target_include_directories(imgui PUBLIC .)
target_link_libraries(imgui PUBLIC glfw)