#pragma once

#include <windows.h>
#include <string>
#include "Button.h"
#include "TextField.h"
#include "GaussKernel.h"
#include <vector>
#include "ComboBox.h"
#include <functional>
#include <shobjidl.h>
#include <shlobj.h>
#include <iostream>
#include "Slider.h"
#include <unordered_map>

class Window {
private:
    WNDCLASSEX windowClass;
    HWND windowHandle;
    std::vector<Control*> controls;
    void setUpControls();
    TextField* inputFileTextField;
    TextField* outputFolderTextField;
    TextField* generalMessagesTextField;
    Slider* sigmaChooseSlider;
    Slider* valueMultiplicationChooseSlider;
    ComboBox* procedureChooseComboBox;

    HFONT font;
    GaussKernel gaussKernel;


public:
    Window(HINSTANCE hInstance, const wchar_t* className,
 unsigned int sizeX, unsigned int sizeY, const wchar_t* windowTitle);
    HWND& getWindowHandle();
    LPCWSTR getClassName() const;
    HINSTANCE getHInstance() const;
    void selectInputFile();
    void selectOutputFolder();
    void processImage();
    void showMessageBox(const std::string& text );
    void showFilterParameters();
    void updateFilterParameters();

    ~Window();
};

