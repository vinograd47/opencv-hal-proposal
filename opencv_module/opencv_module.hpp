#ifndef __OPENCV_MODULE_HPP__
#define __OPENCV_MODULE_HPP__

#include <opencv2/core.hpp>

namespace cv {

// This function is not thread safe.
void loadHalImpl(const String& halLibName);
String getHalInfo();

void resize(const Mat& src, Mat& dst, int interpolation);
void erode(const Mat& src, Mat& dst, int iterations);

}

#endif
