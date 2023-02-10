//
// Created by kamil on 11.10.2022.
//

#include "Window.h"
#include <stdexcept>
#include <functional>
#include <shobjidl.h>
#include <shlobj.h>
#include "TextField.h"
#include "GaussKernel.h"
#include "Image.h"

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
std::unordered_map<unsigned int, std::function<void()>> callbacks;
wchar_t* convertCharArrayToLPCWSTR(const char* charArray);

Window::Window(HINSTANCE hInstance, const wchar_t* className, unsigned int sizeX, unsigned int sizeY, const wchar_t* windowTitle) : windowClass(),
                                                 gaussKernel(5,1,1000000,128)     {

    windowClass.cbSize = sizeof( WNDCLASSEX );
    windowClass.style = 0;
    windowClass.lpfnWndProc = WndProc;
    windowClass.cbClsExtra = 0;
    windowClass.cbWndExtra = 0;
    windowClass.hInstance = hInstance;
    windowClass.hIcon = LoadIcon(nullptr, IDI_APPLICATION );
    windowClass.hCursor = LoadCursor(nullptr, IDC_ARROW );
    windowClass.hbrBackground =( HBRUSH )(COLOR_WINDOW + 0 );
    windowClass.lpszMenuName = nullptr;
    windowClass.lpszClassName = className;
    windowClass.hIconSm = LoadIcon(nullptr, IDI_APPLICATION );

    if( !RegisterClassEx( & windowClass ) )
    {
        MessageBox( nullptr,
                    L"Window class can not be registered",
                    L"OK",
                    MB_ICONEXCLAMATION | MB_OK );

        throw std::invalid_argument("Window class can not be registered");
    }

    windowHandle = CreateWindowEx( WS_EX_CLIENTEDGE,
                                   className,
                                   windowTitle,
                                   (WS_OVERLAPPEDWINDOW  ^ WS_MAXIMIZEBOX)&~WS_SIZEBOX,
                                   CW_USEDEFAULT,
                                   CW_USEDEFAULT,
                                   sizeX,
                                   sizeY,
                                   nullptr,
                                   nullptr,
                                   hInstance,
                                   nullptr);

    font = CreateFont(18,0,0,0,FW_DONTCARE,FALSE,FALSE,FALSE,DEFAULT_CHARSET,OUT_OUTLINE_PRECIS,
                                 CLIP_DEFAULT_PRECIS,CLEARTYPE_QUALITY, VARIABLE_PITCH,TEXT("Arial"));



    setUpControls();
    showFilterParameters();
}

HWND &Window::getWindowHandle() {
    return windowHandle;
}

LPCWSTR Window::getClassName() const {
    return windowClass.lpszClassName;
}

void Window::setUpControls() {
    

    controls.push_back(new Button(windowHandle, getHInstance(), controls.size(), 30, 30, 140, 25, L"Select input file"));
    callbacks[controls.back()->getId()] = [this](){this->selectInputFile();};

    controls.push_back(new Button(windowHandle, getHInstance(), controls.size(), 30, 65, 140, 25, L"Select output folder"));
    callbacks[controls.back()->getId()] = [this](){this->selectOutputFolder();};

    controls.push_back(new Button(windowHandle, getHInstance(), controls.size(), 500, 500, 140, 25, L"Process image"));
    callbacks[controls.back()->getId()] = [this](){this->processImage();};

    this->inputFileTextField = new TextField(controls.size(), windowHandle, getHInstance(), 200, 30, 550, 25,0);
    controls.push_back(inputFileTextField);

    this->outputFolderTextField = new TextField(controls.size(), windowHandle, getHInstance(), 200, 65, 550, 25, 0);
    controls.push_back(outputFolderTextField);

    this->generalMessagesTextField = new TextField(controls.size(), windowHandle, getHInstance(), 30, 135,
                                                   200, 225, WS_EX_CLIENTEDGE, WS_CHILD | WS_VISIBLE | WS_BORDER);
 

    controls.push_back(generalMessagesTextField);

    this->procedureChooseComboBox = new ComboBox(controls.size(), windowHandle, getHInstance(), 500, 150, 200, 250,
                                                 {L"C++ procedure", L"Assembly procedure"});
    controls.push_back(procedureChooseComboBox);

    this->sigmaChooseSlider = new Slider(controls.size(), windowHandle, getHInstance(), 35, 370, 200, 30, L"0.1", L"2", L"Sigma value", 1, 19);

    controls.push_back(sigmaChooseSlider);
    callbacks[sigmaChooseSlider->getId()] = [this]() {this->updateFilterParameters(); };

    for (const auto& el : controls) {
        SendMessage(el->getHandle(), WM_SETFONT, (WPARAM)font, TRUE);
    }
    
}

HINSTANCE Window::getHInstance() const {
    return windowClass.hInstance;
}

Window::~Window() {
    for (auto & el : controls) {
        delete el;
    }
}

void Window::selectInputFile() {


    OPENFILENAME ofn;
    char filename[200];
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = this->getWindowHandle();
    ofn.lpstrFile = (LPWSTR)filename;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = 200;
    ofn.nFilterIndex = 1;
    ofn.lpstrFilter = L"BMP files\0*.bmp*\0\0";
    GetOpenFileName(&ofn);

    SetWindowText( this->inputFileTextField->getHandle(), (LPCWSTR)filename);
}

static int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    return 0;
}

void Window::selectOutputFolder() {

    TCHAR path[MAX_PATH];

    const char * path_param = "";

    BROWSEINFO bi = { 0 };
    bi.lpszTitle  = L"Browse for folder...";
    bi.ulFlags    = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;
    bi.lpfn       = BrowseCallbackProc;
    bi.lParam     = (LPARAM) path_param;

    LPITEMIDLIST pidl = SHBrowseForFolder ( &bi );

    if ( pidl != 0 ) {
        //get the name of the folder and put it in path
        SHGetPathFromIDList(pidl, path);

        //free memory used
        IMalloc *imalloc = 0;
        if (SUCCEEDED(SHGetMalloc(&imalloc))) {
            imalloc->Free(pidl);
            imalloc->Release();
        }
    }

    else {
        path[0] = '\0';
    }

    SetWindowText( this->outputFolderTextField->getHandle(), path);

}

void Window::processImage() {

    auto inputPath = new char [100];
    memset(inputPath, 0, 100);
    auto outputPath = new char [100];
    memset(outputPath, 0, 100);
    wcstombs(inputPath, inputFileTextField->getCurrentText(), 100);
    wcstombs(outputPath, outputFolderTextField->getCurrentText(), 100);

    if(inputPath[0] == '\0') {
        showMessageBox("Please choose input file!");
        return;
    }

    if (outputPath[0] == '\0') {
        showMessageBox("Please choose output file!");
        return;
    }
        
    Image image;
    image.load(inputPath);
    auto width = image.getWidth();
    auto height = image.getHeight();

    auto pixels = (image.getPixels().get());
    auto kernel = gaussKernel.getKernel();

    auto border = static_cast<int>(gaussKernel.getKernelSize()/2);

    auto newPixels = std::shared_ptr<Color[]>(new Color[height * width]);

    for (size_t i = 0; i < width; i++) {

        for (size_t j = 0; j < height; j++) {

            double weight = 0;
            double sumR = 0;
            double sumG = 0;
            double sumB = 0;

            for (int kernelX = -border; kernelX <= border; kernelX++) {
                for (int kernelY = -border; kernelY <= border; kernelY++) {

                    if (i + kernelX < 0 || i + kernelX >= width || j + kernelY < 0 || j + kernelY >= height) {
                        continue;
                    } else {

                        auto pixel = pixels[i + kernelX + (j + kernelY) * width];
                        weight += gaussKernel.getKernelFragment(kernelX + border, kernelY + border);


                        sumR += (double) pixel.r * gaussKernel.getKernelFragment(kernelX + border, kernelY + border);
                        sumG += (double) pixel.g * gaussKernel.getKernelFragment(kernelX + border, kernelY + border);
                        sumB += (double) pixel.b * gaussKernel.getKernelFragment(kernelX + border, kernelY + border);

                    }
                }
            }

            newPixels[i + j * width].r = (unsigned char)(sumR / weight);
            newPixels[i + j * width].g = (unsigned char)(sumG / weight);
            newPixels[i + j * width].b = (unsigned char)(sumB / weight);
        }
    }

    image.setPixels(newPixels);
    image.save(outputPath);

    delete[] inputPath;
    delete[] outputPath;
}

void Window::showMessageBox(const std::string &text) {

    MessageBoxA(windowHandle, text.c_str(), nullptr, MB_OK | MB_ICONWARNING);
}

void Window::showFilterParameters() {

    std::string result = "Filter parameters:\nKernel size: ";
    result += std::to_string(gaussKernel.getKernelSize());
    result += "    \nSigma value: ";
    result += std::to_string(gaussKernel.getSigma());
    result += "    \nMultiplication value: ";
    result += std::to_string(gaussKernel.getWeightMultiplier());
    result += "\n\nKernel values:\n";

    auto kernelValues = gaussKernel.getKernel();

    for (int i = 0 ; i < gaussKernel.getKernelSize(); i++) {
        for (int j = 0; j < gaussKernel.getKernelSize(); j++) {
            auto temp = std::to_string(kernelValues[j + i * gaussKernel.getKernelSize()]);
            result += temp;
            for (int k = 0; k < 5 - temp.size(); k++) {
                result += " ";
            }
        }
        result += "\n";
    }

    auto final = convertCharArrayToLPCWSTR(result.c_str());

    SetWindowText( this->generalMessagesTextField->getHandle(), final);
    delete [] final;
}

void Window::updateFilterParameters()
{
    this->gaussKernel.setNewSigma(this->sigmaChooseSlider->getValue());
    this->showFilterParameters();
}


LRESULT WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {

    switch( msg )
    {
        case WM_COMMAND:

            if (callbacks.find(wParam) != callbacks.end()) {
                callbacks[wParam]();
            }
            break;

        case WM_CLOSE:
            DestroyWindow( hwnd );
            break;

        case WM_DESTROY:
            PostQuitMessage( 0 );
            break;

        default:
            return DefWindowProc( hwnd, msg, wParam, lParam );
    }

    return 0;
}

wchar_t* convertCharArrayToLPCWSTR(const char* charArray)
{
    wchar_t* wString = new wchar_t[4096];
    MultiByteToWideChar(CP_ACP, 0, charArray, -1, wString, 4096);
    return wString;
}
