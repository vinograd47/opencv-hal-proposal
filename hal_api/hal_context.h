#ifndef __OPENCV_HAL_CONTEXT_H__
#define __OPENCV_HAL_CONTEXT_H__

enum {
    CV_HAL_IND_OPENCV_VERSION,

    // OpenCV 3.0
    CV_HAL_IND_NUM_THREADS,

    // OpenCV 3.1
    CV_HAL_IND_CUDA_STREAM,

    CV_HAL_CONTEXT_SIZE
};

#define CV_HAL_GET_OPENCV_VERSION(context) \
    ((context) ? (int) *(int*)((context) + CV_HAL_IND_OPENCV_VERSION) : 0)

// OpenCV 3.0

#define CV_HAL_GET_NUM_THREADS(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 300 ? *(int*)((context) + CV_HAL_IND_NUM_THREADS) : -1)

// OpenCV 3.1

#define CV_HAL_GET_CUDA_STREAM(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 310 ? (cudaStream_t) (context)[CV_HAL_IND_CUDA_STREAM] : (cudaStream_t) 0)

#endif
