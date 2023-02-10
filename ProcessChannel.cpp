#include "pch.h"
#include "ProcessChannel.h"

unsigned char processChannel(uint8_t* channelToProcess, uint8_t* kernel)
{
    int sum = 0;
    int kernelWeight = 0;
    for (int i = 0; i < 32; i++) {
        kernelWeight += kernel[i];
        sum += channelToProcess[i] * kernel[i];
    }
    return (unsigned char)(sum / kernelWeight);
}
