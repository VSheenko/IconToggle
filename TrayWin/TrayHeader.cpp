#include "TrayHeader.h"

TrayHeader::TrayHeader(HWND const &hWnd, const char* className) {
    configManager = ConfigManager::Instance(CONFIG_PATH);
    configManager->Deserialization();

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

    CreateTrayMenu();

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
    DestroyMenu(hMenu);
    Shell_NotifyIcon(NIM_DELETE, &nid);
}

VOID TrayHeader::CreateTrayMenu() {
    if (hMenu)
        return;

    hMenu = CreatePopupMenu();
    hSubMenu = CreatePopupMenu();

    AppendMenu(hMenu, MF_BYCOMMAND  | MF_CHECKED, TrayHeader::ID_CHECKBOX_LBM, "LBM to Show/Hide");
    AppendMenu(hMenu, MF_BYCOMMAND  | MF_UNCHECKED, TrayHeader::ID_CHECKBOX_SHORTCUT, "Shortcut to Show/Hide");

    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, TrayHeader::ID_BTN_SET_SHORTCUT, "Set Shortcut");
    AppendMenu(hMenu, MF_STRING, TrayHeader::ID_BTN_SET_TIMER, "Set auto hide timer");
    AppendMenu(hMenu, MF_STRING, TrayHeader::ID_BTN_STARTUP, "Set on startup");

    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hSubMenu, MF_STRING, TrayHeader::ID_LINK_GITHUB, "GitHub (Click)");
    AppendMenu(hSubMenu, MF_STRING | MF_DISABLED, 0, "@VSheenko");
    AppendMenu(hMenu, MF_POPUP, (UINT_PTR)(hSubMenu), "About");

    AppendMenu(hMenu, MF_SEPARATOR, 0, NULL);

    AppendMenu(hMenu, MF_STRING, TrayHeader::ID_TRAY_EXIT, "Exit");
}

VOID TrayHeader::DisplayTrayMenu(HWND& hWnd) {
    POINT pt;
    GetCursorPos(&pt);
    SetForegroundWindow(hWnd);
    TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, NULL);
}

BOOL TrayHeader::HandlerTrayMenu(HWND const &hWnd, const WPARAM &wParam) {
    switch (wParam) {
        case TrayHeader::ID_TRAY_EXIT:
            PostMessage(hWnd, WM_CLOSE, 0, 0);
            return TRUE;
        case ID_CHECKBOX_LBM:
        case ID_CHECKBOX_SHORTCUT:
            UpdateCheckBoxBtn(LOWORD(wParam));
            return TRUE;
        case ID_BTN_SET_SHORTCUT:
            // TODO: Create a dialog for setting a shortcut
            return TRUE;
        case ID_BTN_SET_TIMER:
            // TODO: Create a dialog for setting a timer
            return TRUE;
        case ID_LINK_GITHUB:
            ShellExecute(NULL, "open", "https://github.com/VSheenko/", NULL, NULL, SW_SHOWNORMAL);
        default:
            return FALSE;
    }
}

VOID TrayHeader::UpdateCheckBoxBtn(UINT id) {
    if (id == TrayHeader::ID_CHECKBOX_LBM) {
        configManager->isLBM = !configManager->isLBM;
        CheckMenuItem(hMenu, TrayHeader::ID_CHECKBOX_LBM, MF_BYCOMMAND | (configManager->isLBM ? MF_CHECKED : MF_UNCHECKED));
    } else {
        configManager->isShortcut = !configManager->isShortcut;
        CheckMenuItem(hMenu, TrayHeader::ID_CHECKBOX_SHORTCUT, MF_BYCOMMAND | (configManager->isShortcut ? MF_CHECKED : MF_UNCHECKED));
    }

    configManager->Serialization();
}