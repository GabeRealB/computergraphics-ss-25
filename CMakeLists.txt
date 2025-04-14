cmake_minimum_required(VERSION 3.13)
project(app)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(ext)

file(GLOB_RECURSE APP_SRC
    src/*.h
    src/*.cpp
)

add_executable(app ${APP_SRC})
target_link_libraries(app glad glfw glm imgui stb_image)
set_target_properties(app PROPERTIES CXX_STANDARD 17)

if (WIN32)
    target_compile_definitions(app PRIVATE RESOURCE_PATH=L"${CMAKE_CURRENT_SOURCE_DIR}/resources")
else()
    target_compile_definitions(app PRIVATE RESOURCE_PATH="${CMAKE_CURRENT_SOURCE_DIR}/resources")
endif()

if (MSVC)
    target_compile_options(app PRIVATE /W4 /EHsc)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT app)
else()
    target_compile_options(app PRIVATE -Wall -Wextra -pedantic)
endif()
