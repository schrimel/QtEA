#include <chrono>
#include <thread>
#include <iostream>

#include <QMutex>
#include <QDebug>

#include "keyboardagent.h"

#if defined(Q_OS_WIN32)

#include <Windows.h>

HHOOK hHook = {NULL};
bool isCtrlDown = false;
bool isAltDown = false;
bool isWinDown = false;
bool isEscDown = false;
bool isAltGrDown = false;
bool isAllowedCtrlCommand = true;
bool isAllowedAltCommand = true;
bool isAllowedAltGrCommand = true;
//evtl function keys...

KeyboardAgent::KeyboardAgent() : mIsRunning(false)
{
}

void KeyboardAgent::start()
{
    mIsRunning = true;
    hHook = setHook();
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if(nCode >= 0) //microsoft: if nCode is less than zero, the hook procedure must pass on without further processing
    {
        KBDLLHOOKSTRUCT *kbdStruct = (KBDLLHOOKSTRUCT*) lParam;
        DWORD wVirtKey = kbdStruct->vkCode;
        DWORD wScanCode = kbdStruct->scanCode;
        DWORD wFlags = kbdStruct->flags;
        qDebug() << wVirtKey;
        switch(wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            switch(wVirtKey)
            {
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
                isCtrlDown = true;
                break;
            case VK_MENU:
            case VK_RMENU:
                isAltDown = true;
                break;
            case VK_LWIN:
            case VK_RWIN:
                isWinDown = true;
                break;
            case VK_ESCAPE:
                isEscDown = true;
                break;
            case 'C':
            case 'V':
            case 'X':
            case 'A':
                isAllowedCtrlCommand = true;
                break;  //these are handled by qtea and not ll keyboard hook...
            case VK_F4:
                isAllowedAltCommand = true;
                break;
            case '	':
                isAllowedAltCommand = false;
                isAllowedAltGrCommand = false;
                isAllowedCtrlCommand = false;
                break;
            case '7':
            case '8':
            case '9':
            case '0':
            case 'q':
            case 'Q':
            case '+':
            case '?':
                isAllowedAltGrCommand = true;
                break;
            }
            if(isWinDown || isCtrlDown || isAltDown || isEscDown)
            {
                bool suppressSignal = !((isCtrlDown&&isAllowedCtrlCommand) || (isAltDown && isAllowedAltCommand) || (isCtrlDown&&isAltDown&&isAllowedAltGrCommand));
                qDebug() << suppressSignal;
                if(suppressSignal)
                {
                    return (LRESULT) 1; //return non-zero value and do not pass on to next method
                }
            }
          break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            switch(wVirtKey)
            {
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
               isCtrlDown = false;
               isAltGrDown = false;
               break;
            case VK_MENU:
            case VK_RMENU:
               isAltDown = false;
               isAltGrDown = false;
               break;
            case VK_LWIN:
            case VK_RWIN:
               isWinDown = false;
               break;
            case VK_ESCAPE:
               isEscDown = false;
               break;
            case '	':
                isAllowedAltCommand = true;
                isAllowedAltGrCommand = true;
                isAllowedCtrlCommand = true;
                break;
            }
        }
    }
    return CallNextHookEx(hHook,nCode, wParam, lParam);
}

HMODULE GetCurrentModule()
{
    HMODULE wModuleHandle;
    GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, (LPCTSTR) GetCurrentModule, &wModuleHandle);
    return wModuleHandle;
}


HHOOK KeyboardAgent::setHook()
{
   return SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetCurrentModule(), 0);
}

void KeyboardAgent::unsetHook(HHOOK hOld)
{
    UnhookWindowsHookEx(hOld);
}

void KeyboardAgent::terminate()
{
    mIsRunning = false;
    unsetHook(hHook);
}


#elif defined(Q_OS_LINUX)

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <atomic>

std::atomic_bool wIsRunning(false);
Display* appDisplay;

void KeyboardAgent::start()
{
    qDebug() << "started";
    mIsRunning = true;
    wIsRunning = true;
    setHook();
}

void workerFunc()
{
    qDebug() << "hello there";
    Display* display = XOpenDisplay(0);
    Window root DefaultRootWindow(display);
    XEvent evt;

    unsigned int modifiers = AnyModifier;
    int keycode = XKeysymToKeycode(display, XK_C);
    Window grab_window = root;
    Bool owner_events = False;
    int pointer_mode = GrabModeAsync;
    int keyboard_mode = GrabModeAsync;

    XGrabKey(display, keycode, modifiers, grab_window, owner_events, pointer_mode, keyboard_mode);

    XSelectInput(display, root, KeyPressMask);
    while(true)
    {
        if(!wIsRunning.load())
        {
            break;
        }
        XNextEvent(display, &evt);
        switch(evt.type)
        {
        case KeyPress:
            if((evt.xkey.state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask)) == (ShiftMask))
            {
                XUngrabKey(display, keycode, modifiers, grab_window);
            }
            if((evt.xkey.state & (ShiftMask | ControlMask | Mod1Mask | Mod4Mask)) == (Mod4Mask))
            {
                XUngrabKey(display, keycode, modifiers, grab_window);
            }
            break;
        default:
            break;
        }
        //xcxcstd::this_thread::sleep_for(std::chrono::seconds(5));
    }
    XCloseDisplay(0);
}

void KeyboardAgent::setHook()
{
    std::thread t{workerFunc};
    t.detach();
}

void KeyboardAgent::unsetHook()
{
    wIsRunning = false;
    mIsRunning = false;
}

void KeyboardAgent::terminate()
{
    unsetHook();
}

KeyboardAgent::KeyboardAgent() : mIsRunning(false)
{
}
#endif
