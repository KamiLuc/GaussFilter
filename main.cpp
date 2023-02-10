#include <iostream>
#include "GaussKernel.h"
#include <windows.h>
#include "Window.h"

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ) {


    std::string windowClass1 = "WindowClass1";
    MSG command;

    auto window = Window(hInstance, L"window1", 990, 790, L"Gauss filter");

    ShowWindow( window.getWindowHandle(), nCmdShow );
    UpdateWindow( window.getWindowHandle() );


    while (1) {
        while (GetMessage(&command, nullptr, 0, 0))
        {
            try {
                TranslateMessage(&command);
                DispatchMessage(&command);
            }
            catch (...) {

            }
        }
    }

    return command.wParam;
}

