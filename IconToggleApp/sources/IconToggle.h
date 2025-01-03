#ifndef ICONTOGGLE_ICONTOGGLE_H
#define ICONTOGGLE_ICONTOGGLE_H

#include <windows.h>
#include <commctrl.h>
#include "TrayHeader.h"
#include <memory>
#include <mutex>

class IconToggle {
private:
    enum class TimerType : WPARAM {
        GENERAL_TIMER_ID,
        TIMER_AUTO_HIDE_ID,
    };
    static constexpr WPARAM GetTimerID(TimerType type);

    bool timerActivated = false;
    bool autoHided = false;

    HWND GeneralHWnd;
    int  TIMER_INTERVAL_AUTO_HIDE = 5000;
    int  CHECK_INTERVAL = 1000;

    HHOOK hHook = nullptr;
    DWORD lastClickTime = 0;
    DWORD lastDoubleClickTime = 0;
    POINT lastPoint = {0};

    char CLASS_NAME[11] = "IconToggle";

    static std::shared_ptr<IconToggle> IconToggleInst;
    static std::once_flag initFlag;

    std::shared_ptr<TrayHeader> trayHeader;
public:
    constexpr static int WIDTH_DISPLAY_INTERVAL = 100;
    constexpr static int HEIGHT_DISPLAY_INTERVAL = 50;

private:
    explicit IconToggle(const HINSTANCE& hInstance);

    VOID InitGeneralHWND(const HINSTANCE& hInstance);

    static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
    static LRESULT CALLBACK StaticMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);

    VOID DoubleClickHandler();

    HWND FindShellViewWindow();
    VOID AutoHideIcon();

    VOID AutoShowIcon();
    VOID StartTimerAutoHide();

    VOID StopTimerAutoHide();
    BOOL IsClearDesktop();
    BOOL IsActiveShortcut();
    BOOL IsDesktop(const HWND& hWnd);

    BOOL IsVisible();
    VOID EXIT();
public:
    static std::shared_ptr<IconToggle> GetIconToggleInst(const HINSTANCE& hInstance);
    static std::shared_ptr<IconToggle> GetIconToggleInst();

};

#endif //ICONTOGGLE_ICONTOGGLE_H
