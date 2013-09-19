// HAL implementation

#include <iostream>
#include "hal_impl.h"

typedef void* cudaStream_t;

CvHalStatus cvhal_init(CvHalContext context)
{
    std::cout << "Init my HAL" << std::endl;
    std::cout << "OpenCV Version : " << CV_HAL_GET_OPENCV_VERSION(context) << std::endl;
    std::cout << "OpenCV Num Threads : " << CV_HAL_GET_NUM_THREADS(context) << std::endl;
    std::cout << "OpenCV CUDA stream : " << CV_HAL_GET_CUDA_STREAM(context) << "\n" << std::endl;
    return CV_HAL_SUCCESS;
}

const char* cvhal_info()
{
    return "My Custom HAL";
}

CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext context)
{
    if (CV_HAL_GET_NUM_THREADS(context) == 1)
    {
        // the implementation is not effective in single thread
        return CV_HAL_NOT_IMPLEMENTED;
    }

    if (src->depth != CV_HAL_8U || dst->depth != CV_HAL_8U || interpolation != CV_HAL_INTER_LINEAR)
    {
        // the implementation supports only this parameters
        return CV_HAL_NOT_IMPLEMENTED;
    }

    std::cout << "resize from HAL" << std::endl;
    return CV_HAL_SUCCESS;
}

CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext context)
{
    std::cout << "erode from HAL : kernelSize = [" << kernelSize[0] << " x " << kernelSize[1] << "] anchor=(" << anchor[0] << ", " << anchor[1] << ")" << std::endl;
    return CV_HAL_SUCCESS;
}
