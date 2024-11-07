#ifndef ICONTOGGLE_ICONTOGGLE_H
#define ICONTOGGLE_ICONTOGGLE_H

#include <windows.h>
#include <bitset>

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

// TODO: Ему глубоко плевать, что под курсором может быть ярлык, а не рабочий стол
BOOL IsDesktop (HWND hWnd) {
    char windowTitle[256];
    GetWindowText(hWnd, windowTitle, sizeof(windowTitle));

    return hWnd == FindShellViewWindow() || strcmp(windowTitle, "FolderView") == 0;
}

HWND GetHWNDFromCursor() {
    POINT pt;
    GetCursorPos(&pt);
    return WindowFromPoint(pt);
}

#endif //ICONTOGGLE_ICONTOGGLE_H
