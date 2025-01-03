#ifndef ICONTOGGLE_TRAYHEADER_H
#define ICONTOGGLE_TRAYHEADER_H

#include "../resources/resource.h"
#include <windows.h>


class TrayHeader {
private:
    NOTIFYICONDATA nid{};
    bool isVisibility = true;

public:
    static constexpr UINT WM_TRAY_WND = (WM_USER + 1);
    static constexpr UINT ID_TRAY_APP_ICON = 1001;
    static constexpr UINT ID_TRAY_EXIT = 2001;
    static constexpr UINT ID_TRAY_SETTINGS = 2002;


private:
    BOOL InitTrayWnd(const HWND& hWnd, const char* className);

public:
    explicit TrayHeader(const HWND& hWnd, const char* className);
    ~TrayHeader();
    VOID UpdateTrayIcon(bool curVisibility);
};

#endif //ICONTOGGLE_TRAYHEADER_H
