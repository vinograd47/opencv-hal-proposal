#include <iostream>
#include <opencv2/imgproc.hpp>
#include "opencv_module.hpp"

int main(int argc, const char* argv[])
{
    if (argc == 2)
        cv::loadHalImpl(argv[1]);

    std::cout << "HAL info : " << cv::getHalInfo() << std::endl;

    cv::Mat src(128, 128, CV_8UC1);
    cv::Mat dst(64, 64, CV_8UC1);

    std::cout << "\nresize nearest:" << std::endl;
    cv::resize(src, dst, cv::INTER_NEAREST);

    std::cout << "\nresize linear:" << std::endl;
    resize(src, dst, cv::INTER_LINEAR);

    std::cout << "\nerode:" << std::endl;
    cv::erode(src, dst, 1);
}
