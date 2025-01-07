#include <iostream>
#include "IconToggleApp/IconToggle.h"
#include "ConfigManager/ConfigManager.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        std::shared_ptr iconToggle = IconToggle::GetIconToggleInst(hInstance);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }
}
