//
// Created by kamil on 14.10.2022.
//

#include "Slider.h"
#include <winuser.h>
#include <commctrl.h>

Slider::Slider(unsigned int id, HWND windowHandle,
 HINSTANCE hInstance, int xPos, int yPos, unsigned int xSize, unsigned int ySize,
    const wchar_t * leftLabel, const wchar_t* rightLabel, const wchar_t* topLabel,
    unsigned int minValue, unsigned int maxValue) : Control(id) {

    handle = CreateWindowEx(0, TRACKBAR_CLASS, L"TRACKBAR", WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
          xPos, yPos, xSize, ySize, windowHandle, nullptr, hInstance, nullptr);


    this->leftLabel = CreateWindowW(L"Static", leftLabel,
                                    WS_CHILD | WS_VISIBLE, 0, 0, 20, 20, windowHandle, (HMENU)1, nullptr, nullptr);

    SetWindowText(this->leftLabel, leftLabel);

    this->rightLabel = CreateWindowW(L"Static", rightLabel,
                                     WS_CHILD | WS_VISIBLE, 0, 0, 20, 20, windowHandle, (HMENU)2, nullptr, nullptr);

    SetWindowText(this->rightLabel, rightLabel);

    this->topLabel = CreateWindowW(L"Static", topLabel,
        WS_CHILD | WS_VISIBLE, xPos + 20, yPos - 20, xSize, 20, windowHandle, (HMENU)1, nullptr, nullptr);

    SetWindowText(this->topLabel, topLabel);

    SendMessage(handle, TBM_SETRANGE, (WPARAM)true, (LPARAM)MAKELONG(minValue, maxValue));
    SendMessageW(handle, TBM_SETBUDDY, TRUE, (LPARAM)this->leftLabel);
    SendMessageW(handle, TBM_SETBUDDY, FALSE, (LPARAM)this->rightLabel);
}

int Slider::getValue() const
{
    return SendMessage(handle, TBM_GETTIC, NULL, NULL);
}
