#include <chrono>
#include <thread>
#include <iostream>
#include <QMutex>
#include <Windows.h>

#include "keyboardagent.h"

HHOOK hHook = {NULL};
bool isCtrlDown = false;
bool isAltDown = false;
bool isWinDown = false;
bool isEscDown = false;
bool isAltGrDown = false;
bool isAllowedCtrlCommand = false;
bool isAllowedAltCommand = false;
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

        switch(wParam)
        {
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            std::cout << wVirtKey << std::endl;
            if(GetAsyncKeyState(VK_CONTROL) & 0x8000 || GetAsyncKeyState(VK_LCONTROL) & 0x8000 || GetAsyncKeyState(VK_RCONTROL) & 0x8000)
            {
                isCtrlDown = true;
            }
            if(GetAsyncKeyState(VK_MENU) & 0x8000)
            {
                isAltDown = true;
            }
            if(GetAsyncKeyState(VK_ESCAPE) & 0x8000)
            {
                isEscDown = true;
            }
            switch(wVirtKey)
            {
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
                std::cout << "here" << std::endl;
                isCtrlDown = true;
                break;
            case VK_MENU:
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
            }
            if(isWinDown || isCtrlDown || isAltDown || isEscDown)
            {
                bool suppressSignal = !((isCtrlDown&&isAllowedCtrlCommand) || (isAltDown && isAllowedAltCommand));
                if(suppressSignal)
                {
                    isAllowedCtrlCommand = false;
                    return (LRESULT) 1; //return non-zero value and do not pass on to next method
                }
            }
          break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            if(!(GetKeyState(VK_CONTROL) & 0x8000) || !(GetKeyState(VK_LCONTROL) & 0x8000) || !(GetKeyState(VK_RCONTROL) & 0x8000))
            {
                isCtrlDown = false;
            }
            if(!(GetKeyState(VK_MENU) & 0x8000))
            {
                isAltDown = false;
            }
            if(!(GetKeyState(VK_ESCAPE) & 0x8000))
            {
                isEscDown = false;
            }
            switch(wVirtKey)
            {
            case VK_CONTROL:
            case VK_LCONTROL:
            case VK_RCONTROL:
               isCtrlDown = false;
               break;
            case VK_MENU:
               isAltDown = false;
               break;
            case VK_LWIN:
            case VK_RWIN:
               isWinDown = false;
               break;
            case VK_ESCAPE:
               isEscDown = false;
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

void KeyboardAgent::terminate()
{
    mIsRunning = false;
    unsetHook(hHook);
}

HHOOK KeyboardAgent::setHook()
{
   return SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookProc, GetCurrentModule(), 0);
}

void KeyboardAgent::unsetHook(HHOOK hOld)
{
    UnhookWindowsHookEx(hOld);
}
