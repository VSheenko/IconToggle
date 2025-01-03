cmake_minimum_required(VERSION 3.29)
project(IconToggleApp)

set(CMAKE_CXX_STANDARD 23)

add_executable(IconToggle
        IconToggleApp.cpp
        sources/IconToggle.cpp
        sources/IconToggle.h
        sources/TrayHeader.cpp
        sources/TrayHeader.h
        resources/resource.h
)


target_sources(IconToggle PRIVATE
        resources/resources.rc)

set_target_properties(IconToggle PROPERTIES
        COMPILE_FLAGS "-Os -s"
#        LINK_FLAGS "-mwindows -Wl,--gc-sections"
)