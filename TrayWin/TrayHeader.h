#ifndef ICONTOGGLE_TRAYHEADER_H
#define ICONTOGGLE_TRAYHEADER_H

#include "resources/resource.h"
#include <windows.h>


class TrayHeader {
private:
    NOTIFYICONDATA nid{};
    bool isVisibility = true;

    HMENU hMenu = nullptr;
    HMENU hSubMenu = nullptr;

    bool isLBM = true;
    bool isShortcut = false;

public:
    static constexpr UINT WM_TRAY_WND = (WM_USER + 1);
    static constexpr UINT ID_TRAY_APP_ICON = 1001;
    static constexpr UINT ID_TRAY_EXIT = 2001;
    static constexpr UINT ID_CHECKBOX_LBM = 2002;
    static constexpr UINT ID_CHECKBOX_SHORTCUT = 2003;
    static constexpr UINT ID_BTN_SET_SHORTCUT = 2004;
    static constexpr UINT ID_BTN_SET_TIMER = 2005;
    static constexpr UINT ID_BTN_ABOUT = 2006;
    static constexpr UINT ID_LINK_GITHUB = 2007;


private:
    BOOL InitTrayWnd(const HWND& hWnd, const char* className);
    VOID UpdateCheckBoxBtn(UINT id);

public:
    explicit TrayHeader(const HWND& hWnd, const char* className);
    ~TrayHeader();

    VOID CreateTrayMenu();
    VOID DisplayTrayMenu(HWND& hWnd);
    BOOL HandlerTrayMenu(const HWND& hWnd, const WPARAM& wParam);

    VOID UpdateTrayIcon(bool curVisibility);
};

#endif //ICONTOGGLE_TRAYHEADER_H
