cmake_minimum_required(VERSION 3.20)

include(FetchContent)

project(Subnautica2Panel LANGUAGES CXX RC)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Final deliverable folder (single Subnautica2Panel.exe only).
if(NOT S2_OUTPUT_DIR)
    set(S2_OUTPUT_DIR "$ENV{USERPROFILE}/Desktop/sabbbb")
endif()
file(MAKE_DIRECTORY "${S2_OUTPUT_DIR}")

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ProjectVersion.cmake)
option(S2_ENABLE_SIGNING "Authenticode-sign the executable (self-signed may increase detections)" OFF)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/SignExecutable.cmake)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/resources/version.rc.in
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    @ONLY
)

if(MSVC)
    add_compile_options(/W4 /permissive- /utf-8)
    add_compile_definitions(UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
    # Static CRT — no vcruntime140.dll / MSVCP140.dll next to the exe.
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.8
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_DIR ${imgui_SOURCE_DIR})

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
)

add_executable(Subnautica2Panel WIN32
    resources/app.rc
    ${CMAKE_CURRENT_BINARY_DIR}/version.rc
    resources/app.manifest
    src/main.cpp
    src/app/Application.cpp
    src/app/SplashScreen.cpp
    src/app/Window.cpp
    src/gfx/Renderer.cpp
    src/gfx/OceanBackground.cpp
    src/themes/ThemeManager.cpp
    src/functions/FunctionRegistry.cpp
    src/config/IniConfig.cpp
    src/ui/TrainerUI.cpp
    src/ui/Widgets.cpp
    src/ui/WindowChrome.cpp
    src/util/Hotkeys.cpp
    src/util/KeyPoll.cpp
    src/util/LogBuffer.cpp
    ${IMGUI_SOURCES}
)

set_target_properties(Subnautica2Panel PROPERTIES
    OUTPUT_NAME "Subnautica2Panel"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE "${S2_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG "${S2_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${S2_OUTPUT_DIR}"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${S2_OUTPUT_DIR}"
)

target_include_directories(Subnautica2Panel PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
)

target_link_libraries(Subnautica2Panel PRIVATE
    d3d11
    dxgi
    dwmapi
)

if(MSVC)
    target_link_options(Subnautica2Panel PRIVATE
        $<$<CONFIG:Release>:/INCREMENTAL:NO>
        /DYNAMICBASE
        /HIGHENTROPYVA
        /NXCOMPAT
        $<$<CONFIG:Release>:/GUARD:CF>
    )
endif()

if(S2_ENABLE_SIGNING)
    s2_sign_target(Subnautica2Panel)
endif()

add_custom_command(TARGET Subnautica2Panel POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Output: ${S2_OUTPUT_DIR}/Subnautica2Panel.exe"
    COMMENT "Release artifact -> ${S2_OUTPUT_DIR}"
    VERBATIM
)

message(STATUS "Single-file output directory: ${S2_OUTPUT_DIR}")

