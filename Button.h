#pragma once

#include "Control.h"
#include <string>

class Button : public Control {
private:

public:
    Button(HWND windowHandle, HINSTANCE hInstance, unsigned int id, int xPos, int yPos, unsigned int xSize,
        unsigned int ySize,  const wchar_t* text);
};

