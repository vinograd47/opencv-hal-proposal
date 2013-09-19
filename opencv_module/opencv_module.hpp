#ifndef __OPENCV_MODULE_HPP__
#define __OPENCV_MODULE_HPP__

#include <opencv2/core.hpp>

namespace cv {

// This functions allows OpenCV user to control HAL implementation

CV_EXPORTS void loadHalImpl(const String& halLibName);
CV_EXPORTS String getHalInfo();

// OpenCV functions

CV_EXPORTS void resize(const Mat& src, Mat& dst, int interpolation);
CV_EXPORTS void erode(const Mat& src, Mat& dst, int iterations);

}

#endif
