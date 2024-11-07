#include "IconToggle.h"


HHOOK hHook = nullptr;
DWORD lastClickTime = 0;
DWORD lastDoubleClickTime = 0;

LRESULT CALLBACK MouseHookProc(int, WPARAM, LPARAM);
VOID DoubleClickHandler(POINT pos);


VOID DoubleClickHandler (POINT* pos) {
    if (IsDesktop(WindowFromPoint(*pos))) {
        SendMessage(FindShellViewWindow(), WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
    }
}

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION) {
        if (wParam == WM_LBUTTONDOWN) {
            DWORD currentTime = GetTickCount();
            if (currentTime - lastClickTime <= GetDoubleClickTime() &&
                currentTime - lastDoubleClickTime > GetDoubleClickTime()) {

                DoubleClickHandler((POINT*)lParam);

                lastDoubleClickTime = currentTime;
            }
            lastClickTime = currentTime;
        }
    }
    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

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

