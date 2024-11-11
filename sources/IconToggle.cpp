#include "IconToggle.h"
#include "TrayHeader.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GeneralHWnd = InitInstance(hInstance);
    InitTrayWnd(GeneralHWnd);
    UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));

    SetTimer(GeneralHWnd, TIMER_ID, TIMER_INTERVAL, NULL);

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

