#include <windows.h>

void DoMove(int direction)
{
    // Get Desktop area
    RECT rc;
    SystemParametersInfo(SPI_GETWORKAREA, 0, &rc, 0);
    // Get Active Window
    HWND hwnd = GetForegroundWindow();
    if (hwnd == NULL) {
        return;
    }

    // Get maximized state of window
    WINDOWPLACEMENT wnd;
    wnd.length = sizeof(WINDOWPLACEMENT);
    GetWindowPlacement(hwnd, &wnd);

    // Move it left of right
    int half_width = rc.right/2;
    switch(direction) {
    case VK_LEFT:
        if (wnd.showCmd == SW_SHOWMAXIMIZED) { // If window is maximized, restore it
            wnd.showCmd = SW_RESTORE;
            wnd.flags = WPF_ASYNCWINDOWPLACEMENT;
            wnd.rcNormalPosition.left = rc.left;
            wnd.rcNormalPosition.right = half_width;
            wnd.rcNormalPosition.top = rc.top;
            wnd.rcNormalPosition.bottom = rc.bottom;
            SetWindowPlacement(hwnd, &wnd);
        }
        else {
            MoveWindow(hwnd, rc.left, rc.top, half_width, rc.bottom, TRUE);
        }
        break;
    case VK_RIGHT:
        if (wnd.showCmd == SW_SHOWMAXIMIZED) { // If window is maximized, restore it
            wnd.showCmd = SW_RESTORE;
            wnd.flags = WPF_ASYNCWINDOWPLACEMENT;
            wnd.rcNormalPosition.left = half_width;
            wnd.rcNormalPosition.right = half_width;
            wnd.rcNormalPosition.top = rc.top;
            wnd.rcNormalPosition.bottom = rc.bottom;
            SetWindowPlacement(hwnd, &wnd);
        }
        else {
            MoveWindow(hwnd, half_width, rc.top, half_width, rc.bottom, TRUE);
        }
        break;
    case VK_UP:
        ShowWindowAsync(hwnd, SW_MAXIMIZE);
        break;
    case VK_DOWN:
        ShowWindowAsync(hwnd, SW_MINIMIZE);
        break;
    }
}


LRESULT __declspec(dllexport) __stdcall CALLBACK
    Win7KeysHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode == HC_ACTION) {
        int bWinKeyPressed = 0;
        unsigned short keyState1 = GetAsyncKeyState(VK_LWIN);
        unsigned short keyState2 = GetAsyncKeyState(VK_RWIN);
        bWinKeyPressed = (keyState1 | keyState2) & 0xFF00;

        if(bWinKeyPressed) {
            if (wParam == VK_LEFT || wParam == VK_RIGHT || wParam == VK_UP || wParam == VK_DOWN) {
                DoMove(wParam);
            }
        }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
}
