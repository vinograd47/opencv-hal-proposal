// This header defines the OpenCV - HAL interface.
// This header will be used both in OpenCV and in HAL implementation.

// It contains types and constants definitions
// and set of functions prototypes.

// A HAL doesn't have to implement all functions.
// It can only implement a subset.

// This interface can only be extended in new OpenCV versions,
// we can add new functions, but we can't change existed prototypes.

#ifndef __OPENCV_HAL_INTERFACE_H__
#define __OPENCV_HAL_INTERFACE_H__

#include "hal_types.h"

// This two functions must be implemented in all HALs.

CV_HAL_API CvHalStatus cvhal_init(); // initialize HAL
CV_HAL_API const char* cvhal_info(); // Get information about HAL (name, vendor, version, etc.)

// This is a set of HAL functions.
// As it has been said before, a HAL doesn't have to implement all functions.

CV_HAL_API CvHalStatus cvhal_hamming_dist(unsigned char* a, unsigned char* b, size_t len, int* result, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext* context);

#endif
