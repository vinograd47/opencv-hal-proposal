#include <iostream>
#include "hal.hpp"
#include "opencv_module.hpp"

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
        std::cout << "\nround: " << std::endl;
        std::cout << cv::hal::round(1.7) << std::endl;

        Size kernelSize;
        kernelSize.width = kernelSize.height = iterations;

        // First try to call HAL
        if (cv::hal::erode(src, dst, kernel.data, kernelSize, anchor))
            return;

        // If it fails use built-in implementation

        std::cout << "Built-in erode" << std::endl;
    }
}
