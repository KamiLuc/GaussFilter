#pragma once
#include <cstdint>

extern "C" __declspec(dllexport)
unsigned char processChannel(uint8_t* channelToProcess, uint8_t* kernel);