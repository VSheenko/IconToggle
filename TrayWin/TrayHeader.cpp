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

    AppendMenu(hMenu, MF_BYCOMMAND | (ConfigManager::Instance()->isLBM ? MF_CHECKED : MF_UNCHECKED), TrayHeader::ID_CHECKBOX_LBM, "LBM to Show/Hide");
    AppendMenu(hMenu, MF_BYCOMMAND | (ConfigManager::Instance()->isShortcut ? MF_CHECKED : MF_UNCHECKED), TrayHeader::ID_CHECKBOX_SHORTCUT, "Shortcut to Show/Hide");

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
            CreateSetTimerDialog((HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE), hWnd);
            return TRUE;
        case ID_LINK_GITHUB:
            ShellExecute(NULL, "open", "https://github.com/VSheenko/", NULL, NULL, SW_SHOWNORMAL);
        default:
            return FALSE;
    }
}

VOID TrayHeader::UpdateCheckBoxBtn(UINT id) {
    if (id == TrayHeader::ID_CHECKBOX_LBM) {
        ConfigManager::Instance()->isLBM = !ConfigManager::Instance()->isLBM;
        CheckMenuItem(hMenu, TrayHeader::ID_CHECKBOX_LBM, MF_BYCOMMAND | (ConfigManager::Instance()->isLBM ? MF_CHECKED : MF_UNCHECKED));
    } else {
        ConfigManager::Instance()->isShortcut = !ConfigManager::Instance()->isShortcut;
        CheckMenuItem(hMenu, TrayHeader::ID_CHECKBOX_SHORTCUT, MF_BYCOMMAND | (ConfigManager::Instance()->isShortcut ? MF_CHECKED : MF_UNCHECKED));
    }

    ConfigManager::Instance()->Serialization();
}

VOID TrayHeader::CreateSetTimerDialog(HINSTANCE hInstance, HWND parentHwnd) {
    const char INPUT_CLASS_NAME[] = "InputWindowClass";

    static bool isRegistered = false;
    if (!isRegistered) {
        WNDCLASS wc = {};
        wc.lpfnWndProc = WndTimerProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = INPUT_CLASS_NAME;
        wc.hIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_SHOW_ICON), IMAGE_ICON, 32, 32, 0);

        RegisterClass(&wc);
        isRegistered = true;
    }

    HWND inputHwnd = CreateWindowEx(
            0,
            INPUT_CLASS_NAME,
            "Timer setting",
            WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT, 320, 170,
            parentHwnd,
            nullptr,
            hInstance,
            nullptr
    );
    SetWindowLong(inputHwnd, GWL_EXSTYLE, GetWindowLong(inputHwnd, GWL_EXSTYLE) | WS_EX_APPWINDOW);

    if (inputHwnd) {
        ShowWindow(inputHwnd, SW_SHOW);
    }
}

LRESULT CALLBACK TrayHeader::WndTimerProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    static HWND editHwnd;

    switch (msg) {
        case WM_CREATE:
            SetTimerWndInit(hwnd, editHwnd);
            break;
        case WM_DESTROY:
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_COMMAND:
            switch (LOWORD(wparam)) {
                case ID_BTN_SETTER_TIMER_OK: {
                    char time[10];
                    GetWindowText(editHwnd, time, 32);

                    if (!CheckValidTime(time)) {
                        MessageBox(hwnd, "Invalid time", "Error", MB_OK | MB_ICONERROR);
                        break;
                    }

                    ConfigManager::Instance()->autoHideTime = std::stoi(time);
                    ConfigManager::Instance()->Serialization();

                    DestroyWindow(hwnd);
                }
                    break;
                case ID_BTN_SETTER_TIMER_CANCEL:
                    DestroyWindow(hwnd);
                    break;
            }
            break;
        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

VOID TrayHeader::SetTimerWndInit(HWND const &hwnd, HWND& editHwnd) {
    RECT rc;
    GetClientRect(hwnd, &rc);
    int width = rc.right - rc.left;
    int height = rc.bottom - rc.top;

    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    int x = (screenWidth - width) / 2;
    int y = (screenHeight - height) / 2;

    SetWindowPos(hwnd, NULL, x, y, 0, 0, SWP_NOSIZE);

    CreateWindowEx(
            0, "STATIC", "The auto-hide timer for shortcuts is set in seconds. Enter 0 to disable it. Maximum time is 86,400 seconds (24 hours)",
            WS_CHILD | WS_VISIBLE | SS_LEFT,
            10, 10, 300, 50,
            hwnd, nullptr, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr
    );

    editHwnd = CreateWindowEx(
            0, "EDIT", nullptr,
            WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT,
            20, 70, 260, 20,
            hwnd, nullptr, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr
    );
    SetWindowText(editHwnd, std::to_string(ConfigManager::Instance()->autoHideTime).c_str());

    CreateWindowEx(
            0, "BUTTON", "OK",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
            120, 100, 80, 30,
            hwnd, (HMENU)ID_BTN_SETTER_TIMER_OK, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr
    );

    CreateWindowEx(
            0, "BUTTON", "Cancel",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_BORDER,
            210, 100, 80, 30,
            hwnd, (HMENU)ID_BTN_SETTER_TIMER_CANCEL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), nullptr
    );

    SendMessage(hwnd, WM_UPDATEUISTATE, MAKEWPARAM(UIS_SET, UISF_HIDEFOCUS), 0);
}

BOOL TrayHeader::CheckValidTime(const std::string& time) {
    if (time.empty())
        return false;

    for (char c : time) {
        if (!isdigit(c))
            return false;
    }

    try {
        unsigned int timeInt = std::stoi(time);
        if (timeInt > 86400)
            return false;
    } catch (std::exception& e) {
        return false;
    }

    return true;
}
