#include "Control.h"

Control::Control(unsigned int id) : id(id), handle(nullptr) {

}

unsigned int Control::getId() const
{
    return id;
}

Control::~Control() {

}

HWND& Control::getHandle() {
    return handle;
}

