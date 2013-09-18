// This header defines the OpenCV - HAL interface.
// This header will be used both in OpenCV and in HAL implementation.

// It contains types and constants definitions
// and set of functions prototypes.

// A HAL doesn't have to implement all functions.
// It can only implement a subset.

// This interface can only be extended in new OpenCV versions,
// we can add new functions, but we can't change existed prototypes.

#ifndef HAL_INTERFACE_H
#define HAL_INTERFACE_H

#include <stdlib.h>

// HAL has plain C interface

#ifdef __cplusplus
#   define CV_HAL_API extern "C" __attribute__((visibility("default")))
#else
#   define CV_HAL_API __attribute__((visibility("default")))
#endif

// All functions returns error codes, no exceptions

typedef enum _CvHalStatus
{
    CV_HAL_SUCCESS,
    CV_HAL_NOT_IMPLEMENTED
} CvHalStatus;

// HAL has own types and constants
// or maybe we can reuse OpenCV types and constants

enum
{
    CV_HAL_8U,
    CV_HAL_8S,
    CV_HAL_16U,
    CV_HAL_16S,
    CV_HAL_32S,
    CV_HAL_32F,
    CV_HAL_64F
};

enum
{
    CV_HAL_INTER_NN,
    CV_HAL_INTER_LINEAR
};

typedef struct _CvHalMat
{
    unsigned char* data;
    size_t step;
    int rows;
    int cols;

    int depth;
    int channels;

    // for sub-matrices
    unsigned char* datastart;
    int xoff;
    int yoff;
} CvHalMat;

// [width, heght]
typedef int* CvHalSize;

// [x, y]
typedef int* CvHalPoint;

// In addition to input parameters all HAL functions accept Context struct,
// which contains some additional information like:
// * OpenCV version
// * Some parameters (number of threads)
// For CUDA:
// * information about CUDA device
// * stream id

typedef struct _CvHalContext
{
    int opencv_version;
    int num_threads;
} CvHalContext;

// This two functions must be implemented in all HALs.

CV_HAL_API CvHalStatus cvhal_init(); // initialize HAL
CV_HAL_API const char* cvhal_info(); // Get information about HAL (name, vendor, version, etc.)

// This is a set of HAL functions.
// As it has been said before, a HAL doesn't have to implement all functions.

CV_HAL_API CvHalStatus cvhal_hamming_dist(unsigned char* a, unsigned char* b, size_t len, int* result, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext* context);

#endif
