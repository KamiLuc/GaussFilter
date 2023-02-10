#pragma once
#include <windows.h>


class Control {
protected:
    unsigned int id;
    HWND handle;
public:
    explicit Control(unsigned int id);
    unsigned int getId() const;
    HWND& getHandle();
    virtual ~Control();
};
