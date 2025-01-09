#include <iostream>
#include "IconToggleApp/IconToggle.h"
#include "ConfigManager/ConfigManager.h"

#define CONFIG_PATH "config.bin"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        ConfigManager::Instance(CONFIG_PATH);
        std::shared_ptr iconToggle = IconToggle::GetIconToggleInst(hInstance);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
