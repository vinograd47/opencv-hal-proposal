#include <iostream>
#include "opencv_module.hpp"

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
}
