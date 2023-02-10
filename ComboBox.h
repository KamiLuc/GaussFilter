#pragma once

#include <vector>
#include <string>
#include "Control.h"

class ComboBox : public Control {
private:
    std::vector<std::string> texts;

public:
    ComboBox(unsigned int id, HWND windowHandle, HINSTANCE hInstance, int xPos, int yPos, unsigned int xSize,
             unsigned int ySize, const std::vector<const wchar_t*> &texts);
    char getCurrentText() const;
};

