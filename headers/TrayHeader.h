#ifndef ICONTOGGLE_TRAYHEADER_H
#define ICONTOGGLE_TRAYHEADER_H

#include <windows.h>

#define ID_TRAY_APP_ICON 1001
#define WM_TRAY_WND (WM_USER + 1)
#define ID_TRAY_EXIT 2001
#define ID_TRAY_SETTINGS 2002

HINSTANCE hInst;
NOTIFYICONDATA nid;
const char CLASS_NAME[] = "ToggleIcon";

VOID    EXIT();
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
HWND    InitInstance(HINSTANCE);
BOOL    InitTrayWnd(HWND);

// ----------------- Tray -----------------

VOID EXIT() {
    Shell_NotifyIcon(NIM_DELETE, &nid);
    PostQuitMessage(0);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case WM_TRAY_WND:
            if (lparam == WM_RBUTTONUP) {
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

        case WM_DESTROY:
            EXIT();
            break;

        default:
            return DefWindowProc(hwnd, msg, wparam, lparam);
    }
    return 0;
}

HWND InitInstance(HINSTANCE hInstance) {
    hInst = hInstance;

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

BOOL InitTrayWnd(HWND hWnd) {
    ZeroMemory(&nid, sizeof(nid));
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = ID_TRAY_APP_ICON;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAY_WND;
    nid.hIcon = (HICON)LoadImage(NULL, "../resources/icons/show_icon.ico", IMAGE_ICON, 96, 96, LR_LOADFROMFILE);
    strncpy_s(nid.szTip, CLASS_NAME, ARRAYSIZE(nid.szTip));

    Shell_NotifyIcon(NIM_ADD, &nid);
    return TRUE;

}


#endif //ICONTOGGLE_TRAYHEADER_H
