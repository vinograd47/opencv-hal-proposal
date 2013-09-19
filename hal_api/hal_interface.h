// This header defines the OpenCV HAL interface.
// This header will be used both in OpenCV and in HAL implementation.

// It contains a set of functions prototypes.
// All funcions has plain C interface
// All functions returns error codes, no exceptions

// In addition to input parameters all HAL functions accept CvHalContext struct,
// which contains some additional information like:
// * OpenCV version
// * Some parameters (number of threads)
// For CUDA:
// * information about CUDA device
// * stream id

// A HAL doesn't have to implement all functions.
// It can only implement a subset.

// This interface can only be extended in new OpenCV versions,
// we can add new functions, but we can't change existed prototypes.

#ifndef __OPENCV_HAL_INTERFACE_H__
#define __OPENCV_HAL_INTERFACE_H__

#include "hal_types.h"

// This two functions must be implemented in all HALs.

CV_HAL_API CvHalStatus cvhal_init(CvHalContext* context); // initialize HAL
CV_HAL_API const char* cvhal_info(); // Get information about HAL (name, vendor, version, etc.)

// This is the set of HAL functions.

CV_HAL_API CvHalStatus cvhal_hamming_dist(unsigned char* a, unsigned char* b, size_t len, int* result, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext* context);

#endif
