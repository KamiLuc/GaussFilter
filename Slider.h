#pragma once

#include "Control.h"
#include <string>

class Slider : public Control {
private:
    HWND leftLabel;
    HWND rightLabel;
    HWND topLabel;
public:
    Slider(unsigned int id, HWND windowHandle,
 HINSTANCE hInstance, int xPos, int yPos, unsigned int xSize, unsigned int ySize, const wchar_t * leftLabel, const wchar_t* rightLabel, const wchar_t* topLabel, unsigned int minValue, unsigned int maxValue);

    int getValue() const;
};