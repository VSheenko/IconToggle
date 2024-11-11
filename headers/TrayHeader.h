#ifndef ICONTOGGLE_TRAYHEADER_H
#define ICONTOGGLE_TRAYHEADER_H

#include "resource.h"
#include <windows.h>


#define ID_TRAY_APP_ICON 1001
#define WM_TRAY_WND (WM_USER + 1)
#define ID_TRAY_EXIT 2001
#define ID_TRAY_SETTINGS 2002


NOTIFYICONDATA nid;
const char     CLASS_NAME[] = "IconToggle";
bool           isVisibility = true;

BOOL    InitTrayWnd(const HWND&);
VOID    UpdateTrayIcon(const HWND& hWnd);

// ----------------- Tray -----------------

BOOL InitTrayWnd(const HWND& hWnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_WND;
    nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(IDI_SHOW_ICON),
                                 IMAGE_ICON, 96, 96, 0);

    strncpy_s(nid.szTip, CLASS_NAME, ARRAYSIZE(nid.szTip));

    Shell_NotifyIcon(NIM_ADD, &nid);
    return TRUE;
}

VOID UpdateTrayIcon(const HWND& hWnd) {
    LONG_PTR style = GetWindowLongPtr(hWnd, GWL_STYLE);
    bool curVisibility = style & WS_VISIBLE;

    if (curVisibility == isVisibility)
        return;

    isVisibility = curVisibility;
    nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(curVisibility ? IDI_SHOW_ICON : IDI_HIDE_ICON),
                                 IMAGE_ICON, 96, 96, 0);

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

#endif //ICONTOGGLE_TRAYHEADER_H
