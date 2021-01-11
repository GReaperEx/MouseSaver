#ifndef PROGRAM_DATA_H
#define PROGRAM_DATA_H

#include <fstream>
#include <sstream>

struct ProgramData
{
    Hotkey::Data toggleHotkey;
    bool save;
    bool promptForSave;

    int hours;
    int mins;
    int secs;
    int millis;

    int mouseButton;
    int clickType;

    int clickRepeat;
    int repeatTimes;

    bool randTime;
    bool randPos;
    int maxDev;
    bool updownDelay;

    int cursorPos;
    int curX;
    int curY;

    bool operator== (const ProgramData& other) const {
        return toggleHotkey == other.toggleHotkey &&
               save == other.save &&
               promptForSave == other.promptForSave &&
               hours == other.hours && mins == other.mins && secs == other.secs && millis == other.millis &&
               mouseButton == other.mouseButton && clickType == other.clickType &&
               clickRepeat == other.clickRepeat && repeatTimes == other.repeatTimes &&
               randTime == other.randTime && randPos == other.randPos && maxDev == other.maxDev && updownDelay == other.updownDelay &&
               cursorPos == other.cursorPos && curX == other.curX && curY == other.curY;
    }

    bool operator!= (const ProgramData& other) const {
        return !(*this == other);
    }

    void grabData(HWND hwnd) {
        char buffer[64];

        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_HOURS), buffer, sizeof(buffer));
        hours = stoi(std::string(buffer));
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_MINS), buffer, sizeof(buffer));
        mins = stoi(std::string(buffer));
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_SECS), buffer, sizeof(buffer));
        secs = stoi(std::string(buffer));
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_MILLIS), buffer, sizeof(buffer));
        millis = stoi(std::string(buffer));

        mouseButton = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_COMBO_BUTTON));
        clickType = ComboBox_GetCurSel(GetDlgItem(hwnd, IDC_COMBO_TYPE));

        if (Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_HOLD))) {
            clickRepeat = 0;
        } else if (Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_REPEAT))) {
            clickRepeat = 1;
        } else if (Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_INFREPEAT))) {
            clickRepeat = 2;
        }
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_RTIMES), buffer, sizeof(buffer));
        repeatTimes = stoi(std::string(buffer));

        randTime = Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDINT));
        randPos = Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS));
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_RANDDEV), buffer, sizeof(buffer));
        maxDev = stoi(std::string(buffer));
        updownDelay = Button_GetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_DELAY));

        if (Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_CURPOS))) {
            cursorPos = 0;
        } else if (Button_GetCheck(GetDlgItem(hwnd, IDC_RADIO_CUSTOMPOS))) {
            cursorPos = 1;
        }

        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_XPOS), buffer, sizeof(buffer));
        curX = stoi(std::string(buffer));
        Edit_GetText(GetDlgItem(hwnd, IDC_EDIT_YPOS), buffer, sizeof(buffer));
        curY = stoi(std::string(buffer));

        if (maxDev == 0) {
            randPos = false;
        }
    }

    void placeData(HWND hwnd) {
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_HOURS), std::to_string(hours).c_str());
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_MINS), std::to_string(mins).c_str());
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_SECS), std::to_string(secs).c_str());
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_MILLIS), std::to_string(millis).c_str());

        ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_COMBO_BUTTON), mouseButton);
        ComboBox_SetCurSel(GetDlgItem(hwnd, IDC_COMBO_TYPE), clickType);

        Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_HOLD), clickRepeat == 0);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_HOURS), clickRepeat != 0);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_MINS), clickRepeat != 0);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_SECS), clickRepeat != 0);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_MILLIS), clickRepeat != 0);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_RANDDEV), clickRepeat != 0 && randPos);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_HOURS), clickRepeat != 0);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_MINS), clickRepeat != 0);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_SECS), clickRepeat != 0);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_MILLIS), clickRepeat != 0);
        Static_Enable(GetDlgItem(hwnd, IDC_GROUP_INTERVAL), clickRepeat != 0);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_RANDDEV), clickRepeat != 0 && randPos);
        Static_Enable(GetDlgItem(hwnd, IDC_GROUP_RAND), clickRepeat != 0 || clickType == 1);
        Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_RANDINT), clickRepeat != 0);
        Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS), clickRepeat != 0);
        Button_Enable(GetDlgItem(hwnd, IDC_CHECKBOX_DELAY), clickRepeat != 0 || clickType == 1);

        Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_REPEAT), clickRepeat == 1);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_RTIMES), clickRepeat == 1);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_REPEAT), clickRepeat == 1);

        Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_INFREPEAT), clickRepeat == 2);

        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_RTIMES), std::to_string(repeatTimes).c_str());

        Button_SetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDINT), randTime);
        Button_SetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_RANDPOS), randPos);
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_RANDDEV), std::to_string(maxDev).c_str());
        Button_SetCheck(GetDlgItem(hwnd, IDC_CHECKBOX_DELAY), updownDelay);

        Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_CURPOS), cursorPos == 0);
        Button_SetCheck(GetDlgItem(hwnd, IDC_RADIO_CUSTOMPOS), cursorPos == 1);
        Button_Enable(GetDlgItem(hwnd, IDC_BUTTON_PICKLOC), cursorPos == 1);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_XPOS), cursorPos == 1);
        Edit_Enable(GetDlgItem(hwnd, IDC_EDIT_YPOS), cursorPos == 1);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_XPOS), cursorPos == 1);
        Static_Enable(GetDlgItem(hwnd, IDC_STATIC_YPOS), cursorPos == 1);

        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_XPOS), std::to_string(curX).c_str());
        Edit_SetText(GetDlgItem(hwnd, IDC_EDIT_YPOS), std::to_string(curY).c_str());

        Button_SetText(GetDlgItem(hwnd, IDC_BUTTON_START), (std::string("Start(") + toggleHotkey.toString() + ")").c_str());
        Button_SetText(GetDlgItem(hwnd, IDC_BUTTON_STOP), (std::string("Stop(") + toggleHotkey.toString() + ")").c_str());
    }

    void saveToFile(const std::string& filepath) const {
        std::ofstream outfile(filepath);

        outfile << "promptForSave " << promptForSave << std::endl;
        outfile << "save " << save << std::endl;
        outfile << "toggleHotkey " << toggleHotkey.ctrlPressed << ' '
                                   << toggleHotkey.altPressed << ' '
                                   << toggleHotkey.shiftPressed << ' '
                                   << toggleHotkey.keycode << std::endl;
        outfile << "hours " << hours << std::endl;
        outfile << "mins " << mins << std::endl;
        outfile << "secs " << secs << std::endl;
        outfile << "millis " << millis << std::endl;
        outfile << "mouseButton " << mouseButton << std::endl;
        outfile << "clickType " << clickType << std::endl;
        outfile << "clickRepeat " << clickRepeat << std::endl;
        outfile << "repeatTimes " << repeatTimes << std::endl;
        outfile << "randTime " << randTime << std::endl;
        outfile << "randPos " << randPos << std::endl;
        outfile << "maxDev " << maxDev << std::endl;
        outfile << "updownDelay " << updownDelay << std::endl;
        outfile << "cursorPos " << cursorPos << std::endl;
        outfile << "curX " << curX << std::endl;
        outfile << "curY " << curY << std::endl;
    }

    bool loadFromFile(const std::string& filepath) {
        std::ifstream infile(filepath);
        if (!infile.is_open()) {
            return false;
        }

        std::string lineStr;
        while (std::getline(infile, lineStr)) {
            if (lineStr.size() != 0) {
                std::stringstream line(lineStr);

                std::string name;
                line >> name;

                if (name == "save") {
                    line >> save;
                } else if (name == "promptForSave") {
                    line >> promptForSave;
                } else if (name == "toggleHotkey") {
                    line >> toggleHotkey.ctrlPressed >> toggleHotkey.altPressed >> toggleHotkey.shiftPressed >> toggleHotkey.keycode;
                } else if (name == "hours") {
                    line >> hours;
                } else if (name == "mins") {
                    line >> mins;
                } else if (name == "secs") {
                    line >> secs;
                } else if (name == "millis") {
                    line >> millis;
                } else if (name == "mouseButton") {
                    line >> mouseButton;
                } else if (name == "clickType") {
                    line >> clickType;
                } else if (name == "clickRepeat") {
                    line >> clickRepeat;
                } else if (name == "repeatTimes") {
                    line >> repeatTimes;
                } else if (name == "randTime") {
                    line >> randTime;
                } else if (name == "randPos") {
                    line >> randPos;
                } else if (name == "maxDev") {
                    line >> maxDev;
                } else if (name == "updownDelay") {
                    line >> updownDelay;
                } else if (name == "cursorPos") {
                    line >> cursorPos;
                } else if (name == "curX") {
                    line >> curX;
                } else if (name == "curY") {
                    line >> curY;
                }
            }
        }

        return true;
    }
};

#endif // PROGRAM_DATA_H
