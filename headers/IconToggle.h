#ifndef ICONTOGGLE_ICONTOGGLE_H
#define ICONTOGGLE_ICONTOGGLE_H

#include <windows.h>
#include <commctrl.h>
#include "TrayHeader.h"

#define WIDTH_DISPLAY_INTERVAL  100
#define HEIGHT_DISPLAY_INTERVAL 50

bool           timerActivated = false;
bool           autoHided = false;
HWND           GeneralHWnd;
const UINT_PTR TIMER_ID = 1;
const UINT_PTR TIMER_AUTO_HIDE_ID = 2;
const int      TIMER_INTERVAL = 1000; // Таймер для проверки состояния рабочего стола
int            TIMER_INTERVAL_AUTO_HIDE = 5000; // Таймер для автоматического скрытия иконки

VOID    EXIT();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND    InitInstance(const HINSTANCE&);
VOID    AutoHideIcon(const HWND&);
VOID    AutoShowIcon(const HWND&);
BOOL    IsClearDesktop();
HWND    FindShellViewWindow();
BOOL    IsDesktop(const HWND&);
BOOL    IsShortcut(const HWND&);
VOID    DoubleClickHandler();
LRESULT CALLBACK MouseHookProc(int, WPARAM, LPARAM);


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
    autoHided = false;

    SendMessage(FindShellViewWindow(), WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
    UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));
}

BOOL IsClearDesktop() {
    HWND hShellView = FindShellViewWindow();
    HWND hSysListView = GetWindow(hShellView, GW_CHILD);

    RECT rect = {0};
    GetWindowRect(hSysListView, &rect);

    for (int i = 0; i < rect.right / WIDTH_DISPLAY_INTERVAL; i++) {
        for (int j = 0; j < rect.bottom / HEIGHT_DISPLAY_INTERVAL; j++) {
            HWND hWnd = WindowFromPoint({i * WIDTH_DISPLAY_INTERVAL, j * HEIGHT_DISPLAY_INTERVAL});
            if (hWnd != hShellView && hWnd != hSysListView)
                return FALSE;

        }
    }

    return TRUE;
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

     if (autoHided && wParam == WM_MOUSEMOVE) {
        AutoShowIcon(FindShellViewWindow());
        UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));

        if (timerActivated) {
            KillTimer(GeneralHWnd, TIMER_AUTO_HIDE_ID);
            timerActivated = false;
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}



LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_TRAY_WND:
            if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
                HMENU hMenu = CreatePopupMenu();
                AppendMenu(hMenu, MF_STRING, ID_TRAY_SETTINGS, "Show Settings");
                AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);
                AppendMenu(hMenu, MF_STRING, ID_TRAY_EXIT, "Exit");

                POINT pt;
                GetCursorPos(&pt);
                SetForegroundWindow(hwnd);
                TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hwnd, NULL);
                DestroyMenu(hMenu);
            }
            break;

        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case ID_TRAY_SETTINGS:
                    MessageBox(hwnd, "This is a Settings", "Settings", MB_OK);
                    break;
                case ID_TRAY_EXIT:
                    EXIT();
                    break;
            }
            break;

        case WM_TIMER:
            if (wparam == 1) {
                UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));

                if (!isVisibility) {
                    if (timerActivated) {
                        KillTimer(GeneralHWnd, TIMER_AUTO_HIDE_ID);
                        timerActivated = false;
                    }

                    return DefWindowProc(hwnd, msg, wparam, lparam);
                }

                if (!TIMER_INTERVAL_AUTO_HIDE || autoHided || !IsClearDesktop()) {
                    if (timerActivated) {
                        KillTimer(GeneralHWnd, TIMER_AUTO_HIDE_ID);
                        timerActivated = false;
                    }

                    return DefWindowProc(hwnd, msg, wparam, lparam);
                }

                if (!timerActivated) {
                    SetTimer(GeneralHWnd, TIMER_AUTO_HIDE_ID, TIMER_INTERVAL_AUTO_HIDE, NULL);
                    timerActivated = true;
                }
            }
            else if (wparam == 2) {
                KillTimer(GeneralHWnd, TIMER_AUTO_HIDE_ID);
                timerActivated = false;

                AutoHideIcon(FindShellViewWindow());
                UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));
            }
            break;

        case WM_DESTROY:
            EXIT();
            break;

        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

HWND InitInstance(const HINSTANCE& hInstance) {
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
            0,
            CLASS_NAME,
            NULL,
            WS_OVERLAPPEDWINDOW,
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
            NULL,
            NULL,
            hInstance,
            NULL
    );

    return hwnd;
}

VOID EXIT() {
    KillTimer(GeneralHWnd, TIMER_ID);
    Shell_NotifyIcon(NIM_DELETE, &nid);
    PostQuitMessage(0);
}

VOID AutoHideIcon(const HWND& hShellView) {
    LONG_PTR style = GetWindowLongPtr(GetWindow(hShellView, GW_CHILD), GWL_STYLE);
    bool curVisibility = style & WS_VISIBLE;

    if (curVisibility) {
        SendMessage(hShellView, WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
        autoHided = true;
    }
}

VOID AutoShowIcon(const HWND& hShellView) {
    LONG_PTR style = GetWindowLongPtr(GetWindow(hShellView, GW_CHILD), GWL_STYLE);
    bool curVisibility = style & WS_VISIBLE;

    if (!curVisibility && autoHided) {
        SendMessage(hShellView, WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
        autoHided = false;
    }
}
#endif //ICONTOGGLE_ICONTOGGLE_H
