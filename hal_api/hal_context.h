// This header defines macroses for context accessing.

// We can add new entries to context in new OpenCV versions,
// so this macroses checks current OpenCV version and
// returns default values for old releases.

#ifndef __OPENCV_HAL_CONTEXT_H__
#define __OPENCV_HAL_CONTEXT_H__

typedef void* cudaStream_t;

typedef struct _CvHalContext {
    int opencv_version;

    // OpenCV 3.0
    int num_threads;

    // OpenCV 3.1
    cudaStream_t cuda_stream;
} CvHalContext;

#define CV_HAL_GET_OPENCV_VERSION(context) \
    ((context) ? context->opencv_version : 0)

// OpenCV 3.0

#define CV_HAL_GET_NUM_THREADS(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 300 ? context->num_threads : -1)

// OpenCV 3.1

#define CV_HAL_GET_CUDA_STREAM(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 310 ? context->cuda_stream : (cudaStream_t) 0)

#endif
