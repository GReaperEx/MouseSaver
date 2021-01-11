#ifndef HOTKEY_H
#define HOTKEY_H

#include <windows.h>

#include <string>

struct Hotkey
{
    struct Data
    {
        bool ctrlPressed;
        bool altPressed;
        bool shiftPressed;
        unsigned keycode;

        const char* toString() const {
            static char buffer[64];

            memset(buffer, 0, sizeof(buffer));

            if (ctrlPressed) {
                strcat(buffer, "CTRL+");
            }
            if (shiftPressed) {
                strcat(buffer, "SHIFT+");
            }
            if (altPressed) {
                strcat(buffer, "ALT+");
            }

            switch (keycode)
            {
            case VK_TAB:
                strcat(buffer, "TAB");
            break;
            case VK_RETURN:
                strcat(buffer, "ENTER");
            break;
            case VK_BACK:
                strcat(buffer, "BACKSPACE");
            break;
            case VK_ESCAPE:
                strcat(buffer, "ESC");
            break;
            case VK_INSERT:
                strcat(buffer, "INS");
            break;
            case VK_DELETE:
                strcat(buffer, "DEL");
            break;
            case VK_HOME:
                strcat(buffer, "HOME");
            break;
            case VK_END:
                strcat(buffer, "END");
            break;
            case VK_PRIOR:
                strcat(buffer, "PGUP");
            break;
            case VK_NEXT:
                strcat(buffer, "PGDN");
            break;
            case VK_UP:
                strcat(buffer, "UP");
            break;
            case VK_DOWN:
                strcat(buffer, "DOWN");
            break;
            case VK_LEFT:
                strcat(buffer, "LEFT");
            break;
            case VK_RIGHT:
                strcat(buffer, "RIGHT");
            break;
            case VK_SPACE:
                strcat(buffer, "SPACE");
            break;
            case VK_SNAPSHOT:
                strcat(buffer, "PRNTSCRN");
            break;
            default:
              {
                UINT c = MapVirtualKey(keycode, MAPVK_VK_TO_CHAR);
                if (keycode >= VK_F1 && keycode <= VK_F24) {
                    strcat(buffer, (std::string("F") + std::to_string(keycode - VK_F1 + 1)).c_str());
                } else if (keycode >= VK_NUMPAD0 && keycode <= VK_DIVIDE) {
                    strcat(buffer, (std::string("NUMPAD_") + (char)c).c_str());
                } else if ((c & 0x80) == 0) {
                    strcat(buffer, (std::string("") + (char)c).c_str());
                } else {
                    buffer[0] = '\0';
                }
              }
            }

            return buffer;
        }

        bool operator== (const Hotkey::Data& other) const {
            return ctrlPressed == other.ctrlPressed &&
                   altPressed == other.altPressed &&
                   shiftPressed == other.shiftPressed &&
                   keycode == other.keycode;
        }
    } data;

private:
    HWND hwnd;
    int keyID;

    bool isReged;

public:
    Hotkey()
    : data{ false, false, false, 0 }, hwnd(NULL), keyID(0), isReged(false)
    {}

    ~Hotkey() {
        unregisterKey();
    }

    void initHotkey(HWND window, int id) {
        if (unregisterKey()) {
            hwnd = window;
            keyID = id;
        }
    }

    bool changeKey(unsigned char code, bool ctrl = false, bool alt = false, bool shift = false) {
        data.ctrlPressed = ctrl;
        data.altPressed = alt;
        data.shiftPressed = shift;
        data.keycode = code;

        return registerKey();
    }

    bool changeKey(const Hotkey::Data& newData) {
        return changeKey(newData.keycode, newData.ctrlPressed, newData.altPressed, newData.shiftPressed);
    }

    bool registerKey() {
        return unregisterKey() &&
               (isReged = RegisterHotKey(hwnd, keyID,
                                          MOD_NOREPEAT |
                                          MOD_ALT*data.altPressed |
                                          MOD_CONTROL*data.ctrlPressed |
                                          MOD_SHIFT*data.shiftPressed,
                                         data.keycode));
    }

    bool unregisterKey() {
        if (!isReged) {
            return true;
        }
        return !(isReged = !UnregisterHotKey(hwnd, keyID));
    }

    const char* toString() const {
        return data.toString();
    }

    const Hotkey::Data& getData() const {
        return data;
    }
};

#endif // HOTKEY_H







































