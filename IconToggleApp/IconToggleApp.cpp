#include <iostream>
#include "sources/IconToggle.h"
#include "sources/TrayHeader.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        std::shared_ptr iconToggle = IconToggle::GetIconToggleInst(hInstance);
    } catch (std::exception& e) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}
