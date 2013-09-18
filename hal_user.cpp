#include <iostream>
#include "core.hpp"
#include "hal.hpp"

namespace cv
{
    // OpenCV functions

    void resize(const Mat& src, Mat& dst, int interpolation)
    {
        // First try to call HAL
        if (cv::hal::resize(src, dst, interpolation))
            return;

        // If it fails use built-in implementation

        std::cout << "Built-in resize" << std::endl;
    }

    void erode(const Mat& src, Mat& dst, const Mat& kernel, int iterations, Point anchor)
    {
        Size kernelSize;
        kernelSize.width = kernelSize.height = iterations;

        // First try to call HAL
        if (cv::hal::erode(src, dst, kernel.data, kernelSize, anchor))
            return;

        // If it fails use built-in implementation

        std::cout << "Built-in erode" << std::endl;
    }
}

int main(int argc, const char* argv[])
{
    if (argc == 2)
        cv::loadHalImpl(argv[1]);

    std::cout << cv::getHalInfo() << std::endl;

    cv::Mat src, dst, kernel;

    std::cout << "\nresize nearest:" << std::endl;
    cv::resize(src, dst, 0);

    std::cout << "\nresize linear:" << std::endl;
    resize(src, dst, 1);

    std::cout << "\nerode:" << std::endl;
    cv::erode(src, dst, kernel, 1, cv::Point());

    std::cout << "\nround: " << std::endl;
    std::cout << cv::hal::round(1.7) << std::endl;
}
