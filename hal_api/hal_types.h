#ifndef __OPENCV_HAL_TYPES_H__
#define __OPENCV_HAL_TYPES_H__

#include <stdlib.h>

#if defined __cplusplus
#   define CV_HAL_INLINE static inline
#elif defined _MSC_VER
#   define CV_HAL_INLINE __inline
#else
#   define CV_HAL_INLINE static
#endif

#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__)
#   define CV_HAL_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#   define CV_HAL_EXPORTS __attribute__ ((visibility ("default")))
#else
#   define CV_HAL_EXPORTS
#endif

#ifdef __cplusplus
#   define CV_HAL_EXTERN_C extern "C"
#else
#   define CV_HAL_EXTERN_C
#endif

// HAL has plain C interface

#define CV_HAL_API CV_HAL_EXTERN_C CV_HAL_EXPORTS

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

#endif
