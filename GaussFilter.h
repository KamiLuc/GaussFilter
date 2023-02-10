#pragma once

#include <stdint.h>
#include "GaussKernel.h"
#include <memory>
#include <barrier>
#include <windows.h>

typedef unsigned char (*processFunc)(volatile uint8_t*, volatile uint8_t*);

enum class Procedure {
	Cpp,
	ASM
};

class GaussFilter {
private:
	 std::shared_ptr<GaussKernel> kernel;
	 int numberOfThreads;
	 Procedure procedure;
	 void processRows(int startRow, int endRow, unsigned char* inputImage, unsigned char* outputImage, volatile int pictureWidth, volatile int pictureHeight);
	 std::atomic<double> lastProcessingTime;
	 std::atomic<unsigned int> threadsReady;
	 std::atomic<unsigned int> threadsDone;
	 HMODULE hCppDll;
	 processFunc cppFunction;
	 HMODULE hAsmDll;
	 processFunc asmFunction;

public:
	GaussFilter();
	std::shared_ptr<GaussKernel> getKernel();
	unsigned char* processImage(unsigned char* data, int width, int height);
	void setAssemblyProcedureOn();
	void setCppAssemblyProcedureOn();
	int getNumberOfThreads() const noexcept { return numberOfThreads; }
	void setNumberOfThreads(int numberOfThreads) { this->numberOfThreads = numberOfThreads; }
	size_t getPixelIndex(int32_t width, int32_t height, int32_t pictureHeight);
	double getLastProcessingTime() const;

	~GaussFilter();
};