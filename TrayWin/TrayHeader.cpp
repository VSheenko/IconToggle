#include "TrayHeader.h"

TrayHeader::TrayHeader(HWND const &hWnd, const char* className) {
    InitTrayWnd(hWnd, className);
}

BOOL TrayHeader::InitTrayWnd(HWND const &hWnd, const char* className) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_WND;
    nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(IDI_SHOW_ICON),
                                 IMAGE_ICON, 96, 96, 0);


    strncpy_s(nid.szTip, className, ARRAYSIZE(nid.szTip));

    Shell_NotifyIcon(NIM_ADD, &nid);
    return TRUE;
}

VOID TrayHeader::UpdateTrayIcon(bool curVisibility) {
    if (curVisibility == isVisibility)
        return;

    isVisibility = curVisibility;
    nid.hIcon = (HICON)LoadImage(GetModuleHandle(NULL),
                                 MAKEINTRESOURCE(curVisibility ? IDI_SHOW_ICON : IDI_HIDE_ICON),
                                 IMAGE_ICON, 96, 96, 0);

    Shell_NotifyIcon(NIM_MODIFY, &nid);
}

TrayHeader::~TrayHeader() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
}
