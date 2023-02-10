

#include "GaussKernel.h"
#include <math.h>
#include <stdexcept>

GaussKernel::~GaussKernel() {
    delete [] kernel;
}

GaussKernel::GaussKernel(int kernelSize, double sigma, int weightMultiplier, int maxValue)
        : sigma(sigma), kernelSize(kernelSize), weightMultiplier(weightMultiplier), maxValue(maxValue) {
    kernel = new unsigned char[kernelSize * kernelSize] {0};
    if (sigma <= 0) {
        setNewSigma(0);
    }

    if (kernelSize <= 0 || kernelSize % 2 == 0) {
        throw std::invalid_argument("Bad kernel size");
    }

    else {
        setNewSigma(sigma);
    }
}

void GaussKernel::setNewSigma(double sigma) {
    this->sigma = sigma;
    updateKernel();
}

void GaussKernel::setNewMultiplierValue(int multiplier)
{
    weightMultiplier = multiplier;
    updateKernel();
}

double GaussKernel::getKernelFragment(int x, int y) const {
    return kernel[calculateKernelIndex(x,y)];
}

int GaussKernel::calculateKernelIndex(int x, int y) const {
    if (x < 0 || x > kernelSize || y < 0 || y > kernelSize) {throw std::invalid_argument("Class: GaussKernel\nMethod: calculateKernelIndex");}
    return x + y * kernelSize;
}

void GaussKernel::updateKernel()
{

    double r, s = 2.0 * sigma * sigma;
    double sum = 0.0;


    int border = static_cast<int>(kernelSize / 2);

    // 5x5 kernel
    for (int x = -border; x <= border; x++) {
        for (int y = -border; y <= border; y++) {
            r = sqrt(x * x + y * y);
            auto tempVal = (exp(-(r * r) / s)) / (3.14 * s);
            kernel[calculateKernelIndex(x + border, y + border)] = tempVal * weightMultiplier;
            sum += tempVal;
        }
    }

    // normalising
    for (int i = 0; i < kernelSize; ++i)
        for (int j = 0; j < kernelSize; ++j) {
            kernel[calculateKernelIndex(i, j)] /= sum;

            if (kernel[calculateKernelIndex(i, j)] > maxValue) {
                kernel[calculateKernelIndex(i, j)] = maxValue;
            }

            if (kernel[calculateKernelIndex(i, j)] == 0) {
                kernel[calculateKernelIndex(i, j)] = 1;
            }
        }

}

void GaussKernel::setKernelFragment(int x, int y, unsigned char value) {
    kernel[calculateKernelIndex(x,y)] = value;
}

unsigned char* GaussKernel::getKernelValues() {
    return kernel;
}

int GaussKernel::getWeightMultiplier() const {
    return weightMultiplier;
}

int GaussKernel::getKernelSize() const {
    return kernelSize;
}

int GaussKernel::getMultiplier() const noexcept
{
    return weightMultiplier;
}

double GaussKernel::getSigma() const noexcept {
    return sigma;
}
