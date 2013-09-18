// HAL implementation

#include "hal_impl.h"
#include <iostream>

CvHalStatus cvhal_init()
{
    std::cout << "Init my HAL" << std::endl;
    return CV_HAL_SUCCESS;
}

const char* cvhal_info()
{
    return "My Custom HAL";
}

CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext* context)
{
    if (context->num_threads == 1)
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

CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext* context)
{
    std::cout << "erode from HAL : kernelSize = [" << kernelSize[0] << " x " << kernelSize[1] << "] anchor=(" << anchor[0] << ", " << anchor[1] << ")" << std::endl;
    return CV_HAL_SUCCESS;
}
