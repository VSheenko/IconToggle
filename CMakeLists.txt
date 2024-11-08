cmake_minimum_required(VERSION 3.29)
project(IconToggle)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH $ENV{PATH_TO_QTBASE})

find_package(Qt6 COMPONENTS
  Core
  Gui
  Widgets
  REQUIRED)

# --------   IconToggle   --------

add_executable(IconToggle
        sources/IconToggle.cpp
        headers/IconToggle.h
)

target_include_directories(IconToggle PRIVATE
        headers
        sources
)

set_target_properties(IconToggle PROPERTIES
        COMPILE_FLAGS "-Os -s"
        LINK_FLAGS "-mwindows -Wl,--gc-sections"
)
# -------- SettingsWindow --------
set(EXEC_SETTINGS_MENU SettingsMenu)
set(SETTINGS_MENU_SOURCES
    main.cpp
    forms/SettingsWindow.ui
    headers/SettingsWindow.h
    sources/SettingsWindow.cpp
)

add_executable(${EXEC_SETTINGS_MENU} ${SETTINGS_MENU_SOURCES})

target_include_directories(${EXEC_SETTINGS_MENU} PRIVATE
    headers
    sources
)

target_link_libraries(${EXEC_SETTINGS_MENU}
  Qt::Core
  Qt::Gui
  Qt::Widgets
)

if (WIN32 AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if(NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif()
    endif()
    if(EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${EXEC_SETTINGS_MENU} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${EXEC_SETTINGS_MENU}>/plugins/platforms/")
        add_custom_command(TARGET ${EXEC_SETTINGS_MENU} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${EXEC_SETTINGS_MENU}>/plugins/platforms/")
    endif()
    foreach(QT_LIB Core Gui Widgets )
        add_custom_command(TARGET ${EXEC_SETTINGS_MENU} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${EXEC_SETTINGS_MENU}>")
    endforeach(QT_LIB)
endif()
