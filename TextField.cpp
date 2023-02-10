
#include "TextField.h"

LPWSTR TextField::getCurrentText() {

    DWORD size = GetWindowTextLength(handle );
    LPWSTR Buffer =( LPWSTR ) GlobalAlloc(GPTR, size + 1 );
    GetWindowText(handle, LPWSTR(Buffer), size + 1);
    return Buffer;
}

TextField::TextField(unsigned int id, HWND windowHandle, HINSTANCE hInstance, unsigned int xPos, unsigned int yPos,
                     int xSize,
                     int ySize, DWORD extendedStyle, DWORD style) : Control(id) {

    handle = CreateWindowEx( extendedStyle, L"STATIC", nullptr, style, xPos, yPos, xSize, ySize, windowHandle, nullptr, hInstance, NULL );
    SetWindowText( handle, L"" );
}

