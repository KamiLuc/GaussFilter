#include "Button.h"

Button::Button(HWND windowHandle, HINSTANCE hInstance, unsigned int id, int xPos, int yPos, unsigned int xSize,
    unsigned int ySize, const wchar_t* text) : Control(id) {

    handle = CreateWindowEx(0, L"BUTTON", text, WS_CHILD | WS_VISIBLE,
        xPos, yPos, xSize, ySize, windowHandle, (HMENU)id, hInstance, nullptr);

}