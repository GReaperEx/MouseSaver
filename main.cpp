#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include "resource.h"
#include "Hotkey.h"
#include "ProgramData.h"

#include <string>
#include <cstdlib>
#include <ctime>

#define ID_HK_TOOLTIP 1
#define ID_HK_TOGGLE  2

#define ID_TM_PROBE 1
#define ID_TM_HKEY  2

HINSTANCE g_hInst;
volatile bool g_started = false;
int g_tooltipOffset[] = { 12, 14 };
Hotkey g_curHotkey;

ProgramData progData, oldProgData;
const ProgramData defaultData = {
    { false, false, false, VK_F10 }, false, true,
    0, 0, 0, 100,
    0, 0, 2, 1,
    false, false, 10, false,
    0, 0, 0
};

LRESULT CALLBACK mainDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK hotkeyDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK saveDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK tooltipDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
    g_hInst = hInstance;

    srand(time(nullptr));

    memset(&progData, 0, sizeof(progData));

    progData = defaultData;
    progData.loadFromFile("MouseSaver.cfg");
    oldProgData = progData;

    DialogBox(hInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, mainDialogProc);

    return 0;
}

void click(unsigned button);
void unclick(unsigned button);

unsigned long holdThread(void*)
{
    DWORD button;
    int sleepTime = progData.hours*3600000 + progData.mins*60000 + progData.secs*1000 + progData.millis;

    if (progData.randTime) {
        sleepTime += rand() % (sleepTime + 1) - sleepTime/2;
    }
    if (progData.updownDelay) {
        sleepTime /= 2;
    }

    switch (progData.mouseButton)
    {
    case 0: // Left click
        button = MOUSEEVENTF_LEFTDOWN;
    break;
    case 1: // Right click
        button = MOUSEEVENTF_RIGHTDOWN;
    break;
    case 2: // Middle click
        button = MOUSEEVENTF_MIDDLEDOWN;
    break;
    }

    if (progData.clickType == 1 && progData.cursorPos != 1) {
        click(button);
        if (progData.updownDelay) {
            Sleep(sleepTime);
        }
        unclick(button);
    }
    click(button);

    while (g_started) {
        Sleep(20);
    }

    unclick(button);

    return 0;
}

unsigned long repeatThread(void* hwnd)
{
    DWORD button;
    int sleepTime = progData.hours*3600000 + progData.mins*60000 + progData.secs*1000 + progData.millis;

    if (progData.randTime) {
        sleepTime += rand() % (sleepTime + 1) - sleepTime/2;
    }
    if (progData.updownDelay) {
        sleepTime /= 2;
    }

    switch (progData.mouseButton)
    {
    case 0: // Left click
        button = MOUSEEVENTF_LEFTDOWN;
    break;
    case 1: // Right click
        button = MOUSEEVENTF_RIGHTDOWN;
    break;
    case 2: // Middle click
        button = MOUSEEVENTF_MIDDLEDOWN;
    break;
    }

    for (int i = 0; i < progData.repeatTimes && g_started; ++i) {
        if (progData.clickType == 1 && progData.cursorPos != 1) {
            click(button);
            if (progData.updownDelay) {
                Sleep(sleepTime);
            }
            unclick(button);
        }
        click(button);
        if (progData.updownDelay) {
            Sleep(sleepTime);
        }
        unclick(button);

        Sleep(sleepTime);
    }

    SendMessage((HWND)hwnd, WM_USER, 0, 0);

    return 0;
}

unsigned long infRepeatThread(void*)
{
    DWORD button;
    int sleepTime = progData.hours*3600000 + progData.mins*60000 + progData.secs*1000 + progData.millis;

    if (progData.randTime) {
        sleepTime += rand() % (sleepTime + 1) - sleepTime/2;
    }
    if (progData.updownDelay) {
        sleepTime /= 2;
    }

    switch (progData.mouseButton)
    {
    case 0: // Left click
        button = MOUSEEVENTF_LEFTDOWN;
    break;
    case 1: // Right click
        button = MOUSEEVENTF_RIGHTDOWN;
    break;
    case 2: // Middle click
        button = MOUSEEVENTF_MIDDLEDOWN;
    break;
    }

    while (g_started) {
        if (progData.clickType == 1 && progData.cursorPos != 1) {
            click(button);
            if (progData.updownDelay) {
                Sleep(sleepTime);
            }
            unclick(button);
        }
        click(button);
        if (progData.updownDelay) {
            Sleep(sleepTime);
        }
        unclick(button);

        Sleep(sleepTime);
    }

    return 0;
}

void click(unsigned button)
{
    int cX, cY;
    if (progData.randPos) {
        cX = progData.curX + rand() % (2*progData.maxDev + 1) - progData.maxDev;
        cY = progData.curY + rand() % (2*progData.maxDev + 1) - progData.maxDev;
    } else {
        cX = progData.curX;
        cY = progData.curY;
    }

    if (progData.cursorPos == 1) {
        POINT point = { progData.curX, progData.curY };
        HWND hwnd = WindowFromPoint(point);

        if (hwnd) {
            RECT winRect;
            GetWindowRect(hwnd, &winRect);

            switch (button)
            {
            case MOUSEEVENTF_LEFTDOWN:
                if (progData.clickType == 0) {
                    SendMessage(hwnd, WM_LBUTTONDOWN, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                } else {
                    SendMessage(hwnd, WM_LBUTTONDBLCLK, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                }
            break;
            case MOUSEEVENTF_RIGHTDOWN:
                if (progData.clickType == 0) {
                    SendMessage(hwnd, WM_RBUTTONDOWN, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                } else {
                    SendMessage(hwnd, WM_RBUTTONDBLCLK, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                }
            break;
            case MOUSEEVENTF_MIDDLEDOWN:
                if (progData.clickType == 0) {
                    SendMessage(hwnd, WM_MBUTTONDOWN, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                } else {
                    SendMessage(hwnd, WM_MBUTTONDBLCLK, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
                }
            break;
            }
        } else {
            GetCursorPos(&point);
            SetCursorPos(cX, cY);
            mouse_event(button, 0, 0, 0, 0);
            SetCursorPos(point.x, point.y);
        }
    } else {
        if (progData.randPos) {
            POINT point;
            GetCursorPos(&point);
            SetCursorPos(cX, cY);
            mouse_event(button, 0, 0, 0, 0);
            SetCursorPos(point.x, point.y);
        } else {
            mouse_event(button, 0, 0, 0, 0);
        }
    }
}

void unclick(unsigned button)
{
    int cX, cY;
    if (progData.randPos) {
        cX = progData.curX + rand() % (2*progData.maxDev + 1) - progData.maxDev;
        cY = progData.curY + rand() % (2*progData.maxDev + 1) - progData.maxDev;
    } else {
        cX = progData.curX;
        cY = progData.curY;
    }

    if (progData.cursorPos == 1) {
        POINT point = { progData.curX, progData.curY };
        HWND hwnd = WindowFromPoint(point);

        if (hwnd) {
            RECT winRect;
            GetWindowRect(hwnd, &winRect);

            switch (button)
            {
            case MOUSEEVENTF_LEFTDOWN:
                SendMessage(hwnd, WM_LBUTTONUP, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
            break;
            case MOUSEEVENTF_RIGHTDOWN:
                SendMessage(hwnd, WM_RBUTTONUP, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
            break;
            case MOUSEEVENTF_MIDDLEDOWN:
                SendMessage(hwnd, WM_MBUTTONUP, 0, (cX - winRect.left) | ((cY - winRect.top) << 16));
            break;
            }
        } else {
            GetCursorPos(&point);
            SetCursorPos(cX, cY);
            mouse_event(button << 1, 0, 0, 0, 0);
            SetCursorPos(point.x, point.y);
        }
    } else {
        if (progData.randPos) {
            POINT point;
            GetCursorPos(&point);
            SetCursorPos(cX, cY);
            mouse_event(button << 1, 0, 0, 0, 0);
            SetCursorPos(point.x, point.y);
        } else {
            mouse_event(button << 1, 0, 0, 0, 0);
        }
    }
}

LRESULT CALLBACK mainDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM)
{
    static bool endProbing;
    static HWND hwndTT;
    static Hotkey probeKey;
    static HANDLE runningThread;

    switch (msg)
    {
    case WM_INITDIALOG:
        ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO_BUTTON), "Left");
        ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO_BUTTON), "Right");
        ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO_BUTTON), "Middle");
        ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO_TYPE), "Single");
        ComboBox_AddString(GetDlgItem(hwnd, IDC_COMBO_TYPE), "Double");

        progData.placeData(hwnd);

        hwndTT = CreateDialog(g_hInst, MAKEINTRESOURCE(IDD_TOOLTIP), hwnd, tooltipDialogProc);

        g_curHotkey.initHotkey(hwnd, ID_HK_TOGGLE);
        g_curHotkey.changeKey(progData.toggleHotkey);
        probeKey.initHotkey(hwnd, ID_HK_TOOLTIP);
        probeKey.data.keycode = VK_ESCAPE;
    break;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDC_BUTTON_HOTKEY:
            DialogBox(g_hInst, MAKEINTRESOURCE(IDD_HOTKEY), hwnd, hotkeyDialogProc);
            Button_SetText(GetDlgItem(hwnd, IDC_BUTTON_START), (string("Start(") + g_curHotkey.toString() + ")").c_str());
            Button_SetText(GetDlgItem(hwnd, IDC_BUTTON_STOP), (string("Stop(") + g_curHotkey.toString() + ")").c_str());
            progData.toggleHotkey = g_curHotkey.getData();
        break;
        case IDC_BUTTON_HELP:
            MessageBox(hwnd, "This may not be helpful, but at least I'm not plugging my premium, ultra-luxe auto-kurikaa with it...", "Help", MB_OK | MB_ICONINFORMATION);
        break;
        case (IDC_COMBO_TYPE | (CBN_SELCHANGE << 16)):
        {
            int clickType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_COMBO_TYPE));
            bool holdButton = Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_HOLD));
            Static_Enable(GetDlgItem(hwnd, IDC_GROUP_RAND), !holdButton || clickType == 1);
            Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_DELAY), !holdButton || clickType == 1);
        }
        break;
        case IDC_RADIO_HOLD:
        case IDC_RADIO_REPEAT:
        case IDC_RADIO_INFREPEAT:
            Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_HOLD), wParam == IDC_RADIO_HOLD);
            Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_REPEAT), wParam == IDC_RADIO_REPEAT);
            Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_INFREPEAT), wParam == IDC_RADIO_INFREPEAT);

            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_RTIMES), wParam == IDC_RADIO_REPEAT);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_REPEAT), wParam == IDC_RADIO_REPEAT);

            Static_Enable(GetDlgItem(hwnd, IDC_GROUP_INTERVAL), wParam != IDC_RADIO_HOLD);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_HOURS), wParam != IDC_RADIO_HOLD);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_HOURS), wParam != IDC_RADIO_HOLD);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_MINS), wParam != IDC_RADIO_HOLD);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_MINS), wParam != IDC_RADIO_HOLD);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_SECS), wParam != IDC_RADIO_HOLD);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_SECS), wParam != IDC_RADIO_HOLD);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_MILLIS), wParam != IDC_RADIO_HOLD);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_MILLIS), wParam != IDC_RADIO_HOLD);

            Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_RANDINT), wParam != IDC_RADIO_HOLD);
            Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS), wParam != IDC_RADIO_HOLD);
          {
            int clickType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_COMBO_TYPE));
            Static_Enable(GetDlgItem(hwnd, IDC_GROUP_RAND), wParam != IDC_RADIO_HOLD || clickType == 1);
            Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_DELAY), wParam != IDC_RADIO_HOLD || clickType == 1);

            BOOL randomPos = Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS));
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_RANDDEV), wParam != IDC_RADIO_HOLD && randomPos);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_RANDDEV), wParam != IDC_RADIO_HOLD && randomPos);
          }
        break;
        case IDC_RADIO_CURPOS:
        case IDC_RADIO_CUSTOMPOS:
            Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_CURPOS), wParam == IDC_RADIO_CURPOS);
            Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_CUSTOMPOS), wParam == IDC_RADIO_CUSTOMPOS);

            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_PICKLOC), wParam == IDC_RADIO_CUSTOMPOS);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_XPOS), wParam == IDC_RADIO_CUSTOMPOS);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_XPOS), wParam == IDC_RADIO_CUSTOMPOS);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_YPOS), wParam == IDC_RADIO_CUSTOMPOS);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_YPOS), wParam == IDC_RADIO_CUSTOMPOS);
        break;
        case IDC_CHECKBOX_RANDPOS:
        {
            BOOL isChecked = Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS));

            isChecked = !isChecked;
            Button_SetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS), isChecked);
            Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_RANDDEV), isChecked);
            Static_Enable(GetDlgItem(hwnd, IDC_STATIC_RANDDEV), isChecked);
        }
        break;
        case IDC_BUTTON_PICKLOC:
        {
            POINT cursorPos;

            GetCursorPos(&cursorPos);
            SetWindowPos(hwndTT, HWND_TOP, cursorPos.x + 16, cursorPos.y + 16, 0, 0, SWP_NOSIZE);
            ShowWindow(hwnd, SW_MINIMIZE);
            ShowWindow(hwndTT, SW_SHOW);
            SetTimer(hwnd, ID_TM_PROBE, 10, (TIMERPROC)NULL);
            endProbing = false;

            probeKey.registerKey();
        }
        break;
        case IDC_BUTTON_START:
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_START), FALSE);
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_STOP), TRUE);
            g_started = true;
            progData.grabData(hwnd);

            switch (progData.clickRepeat)
            {
            case 0:
                runningThread = CreateThread(NULL, 0, holdThread, NULL, 0, NULL);
            break;
            case 1:
                runningThread = CreateThread(NULL, 0, repeatThread, hwnd, 0, NULL);
            break;
            case 2:
                runningThread = CreateThread(NULL, 0, infRepeatThread, NULL, 0, NULL);
            break;
            }
        break;
        case IDC_BUTTON_STOP:
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_START), TRUE);
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_STOP), FALSE);
            g_started = false;

            Sleep(50);
            TerminateThread(runningThread, 0);
            CloseHandle(runningThread);
        break;
        default:
            return FALSE;
        }
    break;
    case WM_TIMER:
        if (endProbing) {
            ShowWindow(hwndTT, SW_HIDE);
            ShowWindow(hwnd, SW_RESTORE);
            KillTimer(hwnd, ID_TM_PROBE);

            POINT cursorPos;
            GetCursorPos(&cursorPos);

            Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_XPOS), (to_string(cursorPos.x)).c_str());
            Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_YPOS), (to_string(cursorPos.y)).c_str());
        } else {
            POINT cursorPos;
            GetCursorPos(&cursorPos);

            RECT rect;
            GetWindowRect(hwndTT, &rect);

            int tooltipDims[] = { rect.right - rect.left, rect.bottom - rect.top };

            if (cursorPos.x + tooltipDims[0] + 12 > GetSystemMetrics(SM_CXSCREEN)) {
                g_tooltipOffset[0] = -tooltipDims[0];
            } else {
                g_tooltipOffset[0] = 12;
            }
            if (cursorPos.y + tooltipDims[1] + 14 > GetSystemMetrics(SM_CYSCREEN)) {
                g_tooltipOffset[1] = -tooltipDims[1];
            } else {
                g_tooltipOffset[1] = 14;
            }

            SetWindowPos(hwndTT, HWND_TOP, cursorPos.x + g_tooltipOffset[0], cursorPos.y + g_tooltipOffset[1], 0, 0, SWP_NOSIZE);
        }
    break;
    case WM_HOTKEY:
        switch (wParam)
        {
        case ID_HK_TOOLTIP:
            probeKey.unregisterKey();
            endProbing = true;
        break;
        case ID_HK_TOGGLE:
            g_started = !g_started;
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_START), !g_started);
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_STOP), g_started);

            if (g_started) {
                progData.grabData(hwnd);
                switch (progData.clickRepeat)
                {
                case 0:
                    runningThread = CreateThread(NULL, 0, holdThread, NULL, 0, NULL);
                break;
                case 1:
                    runningThread = CreateThread(NULL, 0, repeatThread, hwnd, 0, NULL);
                break;
                case 2:
                    runningThread = CreateThread(NULL, 0, infRepeatThread, NULL, 0, NULL);
                break;
                }
            } else {
                Sleep(50);
                TerminateThread(runningThread, 0);
                CloseHandle(runningThread);
            }
        break;
        }
    break;
    case WM_USER:
        Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_START), TRUE);
        Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_STOP), FALSE);
        g_started = false;

        Sleep(50);
        TerminateThread(runningThread, 0);
        CloseHandle(runningThread);
    break;
    case WM_CLOSE:
        progData.grabData(hwnd);
        if (oldProgData != progData) {
            if (progData.promptForSave) {
                DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SAVE), hwnd, saveDialogProc);
            }
        }

        if (!progData.save) {
            oldProgData.save = false;
            oldProgData.promptForSave = progData.promptForSave;
            if (oldProgData != defaultData) {
                oldProgData.saveToFile("MouseSaver.cfg");
            }
        } else {
            progData.saveToFile("MouseSaver.cfg");
        }

        DestroyWindow(hwndTT);
        EndDialog(hwnd, (UINT_PTR)NULL);
        PostQuitMessage(0);
    break;
    default:
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK hotkeyDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM)
{
    static Hotkey::Data newHotkey;

    switch (msg)
    {
    case WM_INITDIALOG:
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_KEYSEL), g_curHotkey.toString());
        newHotkey = g_curHotkey.getData();
    break;
    case WM_TIMER:
    {
        BYTE state[256];

        if (newHotkey.keycode == 0) {
            newHotkey = Hotkey::Data{ false, false, false, 0 };

            GetKeyboardState(state);
            for (int i = 1; i < 256; ++i) {
                if ((state[i] & 0x80) != 0) {
                    if (i == VK_SHIFT || i == VK_LSHIFT || i == VK_RSHIFT) {
                        newHotkey.shiftPressed = true;
                    } else if (i == VK_CONTROL || i == VK_LCONTROL || i == VK_RCONTROL) {
                        newHotkey.ctrlPressed = true;
                    } else if (i == VK_MENU || i == VK_LMENU || i == VK_RMENU) {
                        newHotkey.altPressed = true;
                    } else if (i != VK_LBUTTON && i != VK_MBUTTON && i != VK_RBUTTON && newHotkey.keycode == 0) {
                        newHotkey.keycode = i;
                    }
                }
            }
            if (newHotkey.keycode != 0) {
                Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_CHG), TRUE);

                const char* keyText = newHotkey.toString();
                if (keyText[0] != '\0' && strcmp(keyText, "ESC") != 0) {
                    Button_Enable(GetDlgItem(hwnd, IDOK), TRUE);
                }
                Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_KEYSEL), newHotkey.toString());
                KillTimer(hwnd, ID_TM_HKEY);
            }
        }
    }
    break;
    case WM_COMMAND:
        switch (wParam)
        {
        case IDOK:
            if (newHotkey.keycode != 0) {
                g_curHotkey.changeKey(newHotkey.keycode, newHotkey.ctrlPressed, newHotkey.altPressed, newHotkey.shiftPressed);
            }
        case IDCANCEL:
            EndDialog(hwnd, (UINT_PTR)NULL);
        break;
        case IDC_BUTTON_CHG:
            Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_CHG), FALSE);
            Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_KEYSEL), "");
            Button_Enable(GetDlgItem(hwnd, IDOK), FALSE);
            newHotkey.keycode = 0;
            SetTimer(hwnd, ID_TM_HKEY, 10, (TIMERPROC)NULL);
        break;
        default:
            return FALSE;
        }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, (UINT_PTR)NULL);
    break;
    default:
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK saveDialogProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM)
{
    switch (msg)
    {
    case WM_COMMAND:
        switch (wParam)
        {
        case IDYES:
        case IDNO:
            progData.save = wParam == IDYES;
            progData.promptForSave = !Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_AGAIN));
            EndDialog(hwnd, (UINT_PTR)NULL);
        break;
        default:
            return FALSE;
        }
    break;
    default:
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK tooltipDialogProc(HWND hwnd, UINT msg, WPARAM, LPARAM lParam)
{
    switch (msg)
    {
    case WM_WINDOWPOSCHANGED:
    {
        string toOutput = "Press Esc to finish. CurPos(";
        toOutput += to_string(((WINDOWPOS*)lParam)->x - g_tooltipOffset[0]) + ",";
        toOutput += to_string(((WINDOWPOS*)lParam)->y - g_tooltipOffset[1]) + ")";

        Static_SetText(GetDlgItem(hwnd, IDC_STATIC_TOOLTIP), toOutput.c_str());
    }
    break;
    case WM_CLOSE:
        EndDialog(hwnd, (UINT_PTR)NULL);
    break;
    default:
        return FALSE;
    }

    return TRUE;
}
