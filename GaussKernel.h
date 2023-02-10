#pragma once

class GaussKernel {

private:
    const int kernelSize;
    int weightMultiplier;
    unsigned char* kernel;
    double sigma;
    int maxValue;
    int calculateKernelIndex(int x, int y) const;
    void updateKernel();
public:
    explicit GaussKernel(int kernelSize, double sigma, int weightMultiplier = 100, int maxValue = 127);
    void setNewSigma(double sigma);
    void setNewMultiplierValue(int multiplier);
    double getKernelFragment(int x, int y) const;
    unsigned char* getKernelValues();
    int getWeightMultiplier() const;
    void setKernelFragment(int x, int y, unsigned char value);
    int getKernelSize() const;
    int getMultiplier() const noexcept;
    double getSigma() const noexcept;
    ~GaussKernel();

};
