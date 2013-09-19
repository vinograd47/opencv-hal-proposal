// This header contains types, macroses and constants definitions
// for HAL module.

// HAL has independent from OpenCV interface and uses own types,
// macroses and constants.

#ifndef __OPENCV_HAL_TYPES_H__
#define __OPENCV_HAL_TYPES_H__

#include <stdlib.h>

// CV_HAL_INLINE

#if defined __cplusplus
#   define CV_HAL_INLINE static inline
#elif defined _MSC_VER
#   define CV_HAL_INLINE __inline
#else
#   define CV_HAL_INLINE static
#endif

// CV_HAL_EXPORTS

#if (defined WIN32 || defined _WIN32 || defined WINCE || defined __CYGWIN__) && !defined CV_HAL_STATIC
#   define CV_HAL_EXPORTS __declspec(dllexport)
#elif defined __GNUC__ && __GNUC__ >= 4
#   define CV_HAL_EXPORTS __attribute__ ((visibility ("default")))
#else
#   define CV_HAL_EXPORTS
#endif

// CV_HAL_EXTERN_C

#ifdef __cplusplus
#   define CV_HAL_EXTERN_C extern "C"
#else
#   define CV_HAL_EXTERN_C
#endif

// CV_HAL_API

#define CV_HAL_API CV_HAL_EXTERN_C CV_HAL_EXPORTS

// CvHalStatus

typedef enum _CvHalStatus
{
    CV_HAL_SUCCESS,
    CV_HAL_NOT_IMPLEMENTED
} CvHalStatus;

// Matrix depth

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

// Interpolation mode

enum
{
    CV_HAL_INTER_NEAREST,
    CV_HAL_INTER_LINEAR
};

// CvHalMat

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

// CvHalSize

typedef int* CvHalSize; // [width, heght]

// CvHalPoint

typedef int* CvHalPoint; // [x, y]

// CvHalContext

typedef void** CvHalContext;

#include "hal_context.h"

#endif
