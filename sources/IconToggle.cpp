#include "IconToggle.h"

int WINAPI WinMain(HINSTANCE hinstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hHook = SetWindowsHookEx(WH_MOUSE_LL,
                          MouseHookProc,
                          nullptr,
                          0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
    return 0;
}

