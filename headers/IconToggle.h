#ifndef ICONTOGGLE_ICONTOGGLE_H
#define ICONTOGGLE_ICONTOGGLE_H

#include <windows.h>
#include <commctrl.h>

HWND FindShellViewWindow() {
    HWND hDesktop = GetDesktopWindow();
    HWND hProgman = GetWindow(GetWindow(hDesktop, GW_CHILD), GW_HWNDLAST);
    DWORD threadID = GetWindowThreadProcessId(hProgman, nullptr);

    EnumChildWindows(hDesktop, [](HWND hwnd, LPARAM lParam) -> BOOL {
        DWORD windowThreadID = GetWindowThreadProcessId(hwnd, nullptr);
        TCHAR className[256];

        if (windowThreadID == *(DWORD*)lParam && GetClassName(hwnd, className, sizeof(className) / sizeof(TCHAR))) {
            if (strcmp(className, "SHELLDLL_DefView") == 0) {
                *reinterpret_cast<HWND*>(lParam) = hwnd;
                return FALSE;
            }
        }
        return TRUE;
    }, reinterpret_cast<LPARAM>(&threadID));

    return reinterpret_cast<HWND>(threadID);
}

BOOL IsShortcut (const HWND& hWndLV) {
    return ListView_GetHotItem(hWndLV) != -1;
}

BOOL IsDesktop (const HWND& hWnd) {
    HWND hShellView = FindShellViewWindow();
    HWND hSysListView = GetWindow(hShellView, GW_CHILD);

    if (IsShortcut(hSysListView))
        return FALSE;

    return hWnd == hShellView || hWnd == hSysListView;
}

VOID DoubleClickHandler () {
    SendMessage(FindShellViewWindow(), WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
}

HHOOK hHook = nullptr;
DWORD lastClickTime = 0;
DWORD lastDoubleClickTime = 0;

LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION)
        return CallNextHookEx(hHook, nCode, wParam, lParam);

    if (wParam == WM_LBUTTONUP) {
        if (!IsDesktop(WindowFromPoint(*(POINT*)lParam))) {
            lastDoubleClickTime = 0;
            return CallNextHookEx(hHook, nCode, wParam, lParam);
        }

        DWORD currentTime = GetTickCount();
        if (currentTime - lastClickTime <= GetDoubleClickTime() &&
            currentTime - lastDoubleClickTime > GetDoubleClickTime()) {

            DoubleClickHandler();

            lastDoubleClickTime = currentTime;
        }
        lastClickTime = currentTime;
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

#endif //ICONTOGGLE_ICONTOGGLE_H
