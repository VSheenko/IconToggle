#ifndef ICONTOGGLE_TRAYHEADER_H
#define ICONTOGGLE_TRAYHEADER_H

#include "resources/resource.h"
#include "ConfigManager/ConfigManager.h"
#include <windows.h>


class TrayHeader {
private:
    NOTIFYICONDATA nid{};
    bool isVisibility = true;

    HMENU hMenu = nullptr;
    HMENU hSubMenu = nullptr;

    std::string CONFIG_PATH = "config.bin";
    std::shared_ptr<ConfigManager> configManager;
public:
    static constexpr UINT WM_TRAY_WND = (WM_USER + 1);
    static constexpr UINT ID_TRAY_APP_ICON = 1001;

    enum TrayID : UINT {
        ID_TRAY_EXIT = 2001,
        ID_CHECKBOX_LBM,
        ID_CHECKBOX_SHORTCUT,
        ID_BTN_SET_SHORTCUT,
        ID_BTN_SET_TIMER,
        ID_BTN_ABOUT,
        ID_LINK_GITHUB,
        ID_BTN_STARTUP,
    };

    enum SetTimerDialogID : UINT {
        ID_BTN_SETTER_TIMER_OK = 3001,
        ID_BTN_SETTER_TIMER_CANCEL,
    };


private:
    BOOL InitTrayWnd(const HWND& hWnd, const char* className);
    VOID UpdateCheckBoxBtn(UINT id);

    static LRESULT CALLBACK WndTimerProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

public:
    explicit TrayHeader(const HWND& hWnd, const char* className);
    ~TrayHeader();

    VOID CreateTrayMenu();
    VOID DisplayTrayMenu(HWND& hWnd);
    BOOL HandlerTrayMenu(const HWND& hWnd, const WPARAM& wParam);

    VOID CreateSetTimerDialog(HINSTANCE hInstance, HWND parentHwnd);
    VOID static SetTimerWndInit(const HWND &hwnd, HWND& editHwnd);

    VOID UpdateTrayIcon(bool curVisibility);
};

#endif //ICONTOGGLE_TRAYHEADER_H
