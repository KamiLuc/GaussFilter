#pragma once

#include <string>
#include "control.h"
#include <windows.h>

class TextField : public Control {
private:

public:
    TextField(unsigned int id, HWND windowHandle, HINSTANCE hInstance, unsigned int xPos, unsigned int yPos, int xSize,
              int ySize, DWORD extendedStyle = 0,
              DWORD style = WS_CHILD | WS_VISIBLE | WS_BORDER | SS_CENTERIMAGE | SS_LEFT);
    LPWSTR getCurrentText();
};
