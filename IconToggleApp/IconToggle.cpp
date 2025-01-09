#include <iostream>
#include "IconToggle.h"
#include "TrayWin/TrayHeader.h"

std::shared_ptr<IconToggle> IconToggle::IconToggleInst = nullptr;

void IconToggle::InitGeneralHWND(const HINSTANCE &hInstance) {
    WNDCLASS wc = { 0 };

    wc.lpfnWndProc   = StaticWndProc;
    wc.hInstance     = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    GeneralHWnd = CreateWindowEx(
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
}


LRESULT CALLBACK IconToggle::StaticWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    return IconToggle::GetIconToggleInst()->WndProc(hwnd, msg, wparam, lparam);
}

LRESULT CALLBACK IconToggle::WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
    switch (msg) {
        case TrayHeader::WM_TRAY_WND:
            if (lparam == WM_RBUTTONUP || lparam == WM_LBUTTONUP) {
                trayHeader->DisplayTrayMenu(hwnd);
            }
            break;

        case WM_COMMAND:
            if (trayHeader->HandlerTrayMenu(hwnd, wparam))
                break;

            break;
        case WM_TIMER:
            if (wparam == GetTimerID(TimerType::GENERAL_TIMER_ID)) {
                trayHeader->UpdateTrayIcon(IsVisible());

                if (IsClearDesktop() && !IsActiveShortcut())
                    StartTimerAutoHide();
                else
                    StopTimerAutoHide();
            }
            else if (wparam == GetTimerID(TimerType::TIMER_AUTO_HIDE_ID)) {
                AutoHideIcon();
                StopTimerAutoHide();
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

constexpr WPARAM IconToggle::GetTimerID(IconToggle::TimerType type) {
    return static_cast<WPARAM>(type);
}

std::shared_ptr<IconToggle> IconToggle::GetIconToggleInst(HINSTANCE const &hInstance) {
    if (IconToggleInst == nullptr) {
        new IconToggle(hInstance);
    }

    return IconToggleInst;
}

std::shared_ptr<IconToggle> IconToggle::GetIconToggleInst() {
    if (IconToggleInst == nullptr) {
        throw std::runtime_error("IconToggle instance is not initialized");
    }

    return IconToggleInst;
}

IconToggle::IconToggle(HINSTANCE const &hInstance) {
    IconToggleInst = std::shared_ptr<IconToggle>(this);

    InitGeneralHWND(hInstance);
    trayHeader = std::make_shared<TrayHeader>(GeneralHWnd, CLASS_NAME);

    trayHeader->UpdateTrayIcon(false);

    SetTimer(GeneralHWnd, GetTimerID(TimerType::GENERAL_TIMER_ID), CHECK_INTERVAL, NULL);

    hHook = SetWindowsHookEx(WH_MOUSE_LL,
                             StaticMouseHookProc,
                             nullptr,
                             0);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    UnhookWindowsHookEx(hHook);
}

VOID IconToggle::DoubleClickHandler() {
    if (!ConfigManager::Instance()->isLBM)
        return;

    autoHided = false;

    SendMessage(FindShellViewWindow(), WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
    trayHeader->UpdateTrayIcon(IsVisible());
}

BOOL IconToggle::IsClearDesktop() {
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

HWND IconToggle::FindShellViewWindow() {
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

BOOL IconToggle::IsActiveShortcut() {
    HWND hShellView = FindShellViewWindow();
    HWND hSysListView = GetWindow(hShellView, GW_CHILD);

    return ListView_GetHotItem(hSysListView) != -1;
}

BOOL IconToggle::IsRenamingShortcut() {
    HWND hShellView = FindShellViewWindow();
    HWND hSysListView = GetWindow(hShellView, GW_CHILD);

    return ListView_GetEditControl(hSysListView) != nullptr;
}


BOOL IconToggle::IsDesktop(HWND const &hWnd) {
    HWND hShellView = FindShellViewWindow();
    HWND hSysListView = GetWindow(hShellView, GW_CHILD);

    if (IsActiveShortcut())
        return FALSE;

    return hWnd == hShellView || hWnd == hSysListView;
}

LRESULT CALLBACK IconToggle::StaticMouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    return IconToggle::GetIconToggleInst()->MouseHookProc(nCode, wParam, lParam);
}

LRESULT CALLBACK IconToggle::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode != HC_ACTION)
        return CallNextHookEx(hHook, nCode, wParam, lParam);

    if (wParam == WM_LBUTTONUP) {
        POINT curPoint = *(POINT*)lParam;

        if (!IsDesktop(WindowFromPoint(curPoint))) {
            return CallNextHookEx(hHook, nCode, wParam, lParam);
        }

        DWORD currentTime = GetTickCount();
        if (currentTime - lastClickTime <= GetDoubleClickTime() &&
            currentTime - lastDoubleClickTime > GetDoubleClickTime() &&
            lastPoint.x == curPoint.x && lastPoint.y == curPoint.y) {

            DoubleClickHandler();

            lastDoubleClickTime = currentTime;
        }
        lastClickTime = currentTime;
        lastPoint = curPoint;
    }

    if (autoHided && wParam == WM_MOUSEMOVE) {
        AutoShowIcon();
        trayHeader->UpdateTrayIcon(GetWindow(FindShellViewWindow(), GW_CHILD));
    }

    if (wParam == WM_MOUSEMOVE) {
        StopTimerAutoHide();
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

VOID IconToggle::AutoHideIcon() {
    HWND hShellView = FindShellViewWindow();
    LONG_PTR style = GetWindowLongPtr(GetWindow(hShellView, GW_CHILD), GWL_STYLE);
    bool curVisibility = style & WS_VISIBLE;

    if (curVisibility) {
        SendMessage(hShellView, WM_COMMAND, MAKEWPARAM(29698, 0), NULL);
        trayHeader->UpdateTrayIcon(false);
        autoHided = true;
    }
}

VOID IconToggle::AutoShowIcon() {
    HWND hShellView = FindShellViewWindow();
    LONG_PTR style = GetWindowLongPtr(GetWindow(hShellView, GW_CHILD), GWL_STYLE);
    bool curVisibility = style & WS_VISIBLE;

    if (!curVisibility && autoHided) {
        SendMessage(hShellView, WM_COMMAND, MAKEWPARAM(29698, 0), NULL);\
        trayHeader->UpdateTrayIcon(true);
        autoHided = false;
    }
}

VOID IconToggle::StartTimerAutoHide() {
    if (timerActivated || !ConfigManager::Instance()->autoHideTime || !IsVisible() || IsRenamingShortcut())
        return;

    SetTimer(GeneralHWnd, GetTimerID(TimerType::TIMER_AUTO_HIDE_ID), ConfigManager::Instance()->autoHideTime * 1000, NULL);
    timerActivated = true;
}

VOID IconToggle::StopTimerAutoHide() {
    if (!timerActivated)
        return;

    KillTimer(GeneralHWnd, GetTimerID(TimerType::TIMER_AUTO_HIDE_ID));
    timerActivated = false;
}

VOID IconToggle::EXIT() {
    KillTimer(GeneralHWnd, GetTimerID(TimerType::GENERAL_TIMER_ID));
    KillTimer(GeneralHWnd, GetTimerID(TimerType::TIMER_AUTO_HIDE_ID));
    PostQuitMessage(0);
}

BOOL IconToggle::IsVisible() {
    HWND hShellView = FindShellViewWindow();
    LONG_PTR style = GetWindowLongPtr(GetWindow(hShellView, GW_CHILD), GWL_STYLE);

    return style & WS_VISIBLE;
}
