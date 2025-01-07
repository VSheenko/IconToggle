cmake_minimum_required(VERSION 3.29)
project(IconToggle)

set(CMAKE_CXX_STANDARD 23)

set(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable(IconToggle
        IconToggleApp.cpp
        IconToggleApp/IconToggle.cpp
        IconToggleApp/IconToggle.h
        TrayWin/TrayHeader.cpp
        TrayWin/TrayHeader.h
        ConfigManager/ConfigManager.cpp
        ConfigManager/ConfigManager.h
        resources/resource.h
)

target_include_directories(IconToggle PRIVATE .)

target_sources(IconToggle PRIVATE
        resources/resources.rc)

set_target_properties(IconToggle PROPERTIES
        COMPILE_FLAGS "-Os -s"
        LINK_FLAGS "-mwindows -Wl,--gc-sections"
)