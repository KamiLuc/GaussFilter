#include "GaussFilter.h"
#include <iostream>
#include <fstream>
#include <thread>
#include <vector>


typedef unsigned char (*processFunc)(volatile uint8_t*, volatile uint8_t*);


void GaussFilter::processRows(int startRow, int endRow, unsigned char* inputImage, unsigned char* outputImage,volatile int pictureWidth,volatile int pictureHeight)
{

    processFunc choosenProcedure;
    if (this->procedure == Procedure::Cpp) {
        choosenProcedure = cppFunction;
    }
    else {
        choosenProcedure = asmFunction;
    }

    volatile int imageSize = pictureWidth * pictureHeight * 3;
    volatile int pixels = 0;
    volatile auto kernelSize = this->getKernel()->getKernelSize();
    volatile auto kernel = this->getKernel()->getKernelValues();

    volatile auto border = kernelSize / 2;

    this->threadsReady++;
    while (this->threadsReady != this->getNumberOfThreads()) {
        continue;
    }

    auto channelToProcess = std::make_shared<volatile uint8_t[]>(32);
    auto extendedKernel = std::make_shared<volatile uint8_t[]>(32);

    for (int i = 0; i < 32; i++) {
        channelToProcess[i] = 0;
        extendedKernel[i] = 0;
    }

    for (int i = 0; i < kernelSize * kernelSize; i++) {
        extendedKernel[i] = kernel[i];
    }

    for (volatile size_t xIndex = 0; xIndex < pictureWidth; xIndex++) {
        for (volatile int channel = 0; channel < 3; channel++) {
            for (volatile size_t yIndex = startRow; yIndex < endRow; yIndex++) {
                for (volatile int kernelX = -border; kernelX <= border; kernelX++) {
                    for (volatile int kernelY = -border; kernelY <= border; kernelY++) {
                        if (xIndex + kernelX < 0 || xIndex + kernelX >= pictureWidth || yIndex + kernelY < 0 || yIndex + kernelY >= pictureHeight) {
                            channelToProcess[kernelX + border + (kernelY + border) * kernelSize] = 0;
                        }
                        else {
                            volatile int index = this->getPixelIndex(xIndex + kernelX, yIndex + kernelY, pictureWidth);
                            channelToProcess[kernelX + border + (kernelY + border) * kernelSize] = inputImage[this->getPixelIndex(xIndex + kernelX, yIndex + kernelY, pictureWidth) + channel];
                        }
                    }
                }
                volatile auto v = choosenProcedure(channelToProcess.get(), extendedKernel.get());
                outputImage[this->getPixelIndex(xIndex, yIndex, pictureWidth) + channel] = v;
            }
        }
    }

    this->threadsDone++;
}

GaussFilter::GaussFilter() : numberOfThreads(1), lastProcessingTime(0.0), threadsReady(0), threadsDone(0)
{
	this->kernel = std::make_shared<GaussKernel>(5, 0.1, 10, 128);
    this->procedure = Procedure::Cpp;

    this->hCppDll = LoadLibraryA("CppDll.dll");
    if (this->hCppDll != 0) {
        this->cppFunction = (processFunc)GetProcAddress(this->hCppDll, "processChannel");
    }
    else {
        throw std::runtime_error("No cpp dll");
    }


    this->hAsmDll = LoadLibraryA("AsmDll.dll");
    if (this->hAsmDll != 0) {
        this->asmFunction = (processFunc)GetProcAddress(this->hAsmDll, "processChannel");
    }
    else {
        throw std::runtime_error("No asm dll");
    }
}

std::shared_ptr<GaussKernel> GaussFilter::getKernel()
{
	return kernel;
}

unsigned char* GaussFilter::processImage(unsigned char* data, int pictureWidth, int pictureHeight)
{
    int rowsPerThread = pictureHeight / this->getNumberOfThreads();
    std::vector<std::thread> threads{};
    unsigned char* result = new unsigned char[pictureWidth* pictureHeight * 3];
    this->lastProcessingTime = 0.0;
    this->threadsReady = 0;
    this->threadsDone = 0;

    for (int i = 0; i < this->getNumberOfThreads() - 1; i++) {
        int startRow = i * rowsPerThread;
        int endRow = (i + 1) * rowsPerThread;
        threads.emplace_back(std::thread(&GaussFilter::processRows,
            this,
            startRow,
            endRow,
            data,
            result,
            pictureWidth,
            pictureHeight));
    }

    threads.emplace_back(std::thread(&GaussFilter::processRows,
        this,
        (this->getNumberOfThreads() - 1) * rowsPerThread,
        pictureHeight,
        data,
        result,
        pictureWidth,
        pictureHeight));

    while (this->threadsReady != this->getNumberOfThreads()) {
        continue;
    }

    auto start = std::chrono::high_resolution_clock::now();

    while (this->threadsDone != this->getNumberOfThreads()) {
        continue;
    }

    auto end = std::chrono::high_resolution_clock::now();
    this->lastProcessingTime = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    for (auto& thread : threads) {
        thread.join();
    }

    auto x = (this->lastProcessingTime).load();
    return result;
}

void GaussFilter::setAssemblyProcedureOn()
{
    this->procedure = Procedure::ASM;
}

void GaussFilter::setCppAssemblyProcedureOn()
{
    this->procedure = Procedure::Cpp;
}

size_t GaussFilter::getPixelIndex(int32_t xIndex, int32_t yIndex, int32_t pictureWidth)
{
    auto result = (xIndex * 3 + yIndex * pictureWidth * 3);
    return result;
}

double GaussFilter::getLastProcessingTime() const
{
    return this->lastProcessingTime.load();
}

GaussFilter::~GaussFilter()
{
    FreeLibrary(this->hCppDll);
    FreeLibrary(this->hAsmDll);
}
