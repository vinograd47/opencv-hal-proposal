#include <iostream>
#include <opencv2/imgproc.hpp>
#include "opencv_module.hpp"
#include "hal.hpp"

void cv::resize(const Mat& src, Mat& dst, int interpolation)
{
    // First try to call HAL
    if (hal::resize(src, dst, interpolation))
        return;

    // If it fails use built-in implementation
    std::cout << "Built-in resize" << std::endl;
}

void cv::erode(const Mat& src, Mat& dst, int iterations)
{
    // Use optimized round
    std::cout << hal::round(1.7) << std::endl;

    Mat kernel = getStructuringElement(MORPH_RECT, Size(1 + iterations * 2, 1 + iterations * 2));
    Point anchor(iterations, iterations);

    // First try to call HAL
    if (hal::erode(src, dst, kernel.data, kernel.size(), anchor))
        return;

    // If it fails use built-in implementation
    std::cout << "Built-in erode" << std::endl;
}
