#include <windows.h>

/*  Declare Windows procedure  */
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);
void CreateWndClass(WNDCLASSEX *wincl);

/*  Make the class name into a global variable  */
char szClassName[ ] = "Win7Keys";
HHOOK hhook;
HINSTANCE hInstDLL;

int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance,
    LPSTR lpszArgument, int nCmdShow)
{
    HWND hwnd;               /* This is the handle for our window */
    MSG messages;            /* Here messages to the application are saved */
    WNDCLASSEX wincl;        /* Data structure for the windowclass */

    // Check if we are already running
    HANDLE mutexHandle = CreateMutex(NULL, TRUE, "PravinWin7Keys");
    if (GetLastError() == ERROR_ALREADY_EXISTS) { // We are already running
        MessageBox(NULL, "An instance is already running", "Win7Keys", MB_ICONINFORMATION|MB_OK);
        return 0;
    }

    wincl.hInstance = hThisInstance;
    CreateWndClass(&wincl);
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return 0;

    /* The class is registered, let's create the program*/
    hwnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           szClassName,       /* Title Text */
           WS_OVERLAPPEDWINDOW, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           200,                 /* The programs width */
           200,                 /* and height in pixels */
           HWND_DESKTOP,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hThisInstance,       /* Program Instance handler */
           NULL                 /* No Window Creation data */
           );

    /* Make the window visible on the screen */
    //ShowWindow (hwnd, nCmdShow);

    /* Insert Keyboard HOOK */
    HOOKPROC hkproc;

    hInstDLL = LoadLibrary("Win7Keys.dll");
    if (!hInstDLL) {
        MessageBox(NULL, "Unable to install hook", "Error", MB_OK);
        return 1;
    }
    else {
        hkproc = (HOOKPROC) GetProcAddress(hInstDLL, "Win7KeysHook@12");
        if (!hkproc) {
            MessageBox(NULL, "Unable to find proc", "Error", MB_OK);
            FreeLibrary(hInstDLL);
            return 1;
        }
        else {
            hhook = SetWindowsHookEx(WH_KEYBOARD, hkproc, hInstDLL, 0);
        }
    }

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }

    UnhookWindowsHookEx(hhook);
    FreeLibrary(hInstDLL);
    ReleaseMutex(mutexHandle);
    return messages.wParam;
}

LRESULT CALLBACK WindowProcedure (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        case WM_DESTROY:
            PostQuitMessage (0);
            break;
        default:
            return DefWindowProc (hwnd, message, wParam, lParam);
    }

    return 0;
}

void CreateWndClass(WNDCLASSEX *wincl)
{
    /* The Window structure */
    wincl->lpszClassName = szClassName;
    wincl->lpfnWndProc = WindowProcedure;      /* This function is called by windows */
    wincl->style = CS_DBLCLKS;                 /* Catch double-clicks */
    wincl->cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl->hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wincl->hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wincl->hCursor = LoadCursor (NULL, IDC_ARROW);
    wincl->lpszMenuName = NULL;                 /* No menu */
    wincl->cbClsExtra = 0;                      /* No extra bytes after the window class */
    wincl->cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wincl->hbrBackground = (HBRUSH) COLOR_BACKGROUND;
}
