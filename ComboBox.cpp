//
// Created by kamil on 14.10.2022.
//

#include "ComboBox.h"

ComboBox::ComboBox(unsigned int id, HWND windowHandle, HINSTANCE hInstance, int xPos, int yPos, unsigned int xSize,
                   unsigned int ySize, const std::vector<const wchar_t*> &texts) : Control(id) {

    this->handle = CreateWindowEx( WS_EX_CLIENTEDGE, L"COMBOBOX", nullptr, WS_CHILD | WS_VISIBLE | WS_BORDER |
            CBS_DROPDOWNLIST, xPos, yPos, xSize, ySize, windowHandle, nullptr, hInstance, nullptr );

    for (const auto& el : texts) {
        SendMessage( this->handle, CB_ADDSTRING, 0, (LPARAM)el );
    }

    if (texts.size() != 0) {
        SendMessage( this->handle, CB_SETCURSEL, 0, 0);
    }
}

char ComboBox::getCurrentText() const {

    try {
        DWORD size = GetWindowTextLength(handle);
        LPWSTR Buffer = (LPWSTR)GlobalAlloc(GPTR, size + 1);
        GetWindowText(handle, Buffer, size + 1);
        char result = *Buffer;
        //GlobalFree(Buffer);
        return result;
    }
    catch (...) {
        return 'C';
    }
}
