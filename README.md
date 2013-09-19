OpenCV Hardware Acceleration Layer
==================================

This document describes design for the OpenCV Hardware Acceleration Layer (HAL) interface.

HAL is a separate module that contains optimized implementation of some basic functions. 
It is used to accelerate OpenCV for different platforms.
The HAL implementations can be provided by different third-party developers.

The main goal is that the HAL can be developed independently from OpenCV.
It can have own release process, it can be closed-source.
OpenCV can provide separate test suite for HAL implementations.

OpenCV can be built in three different modes:
* Without HAL support. Only default implementation will be used in this mode.
* With statically-linked HAL. OpenCV will use accelerated function from this HAL.
* With dynamically-loadable HAL support. OpenCV will try to load HAL implementation at runtime, if it fails OpenCV will use default implementation.

OpenCV uses CMake options `HAL_MODE`, `STATIC_HAL_INCLUDE_DIR` and `STATIC_HAL_LIB`, which choose the mode and the HAL library.



HAL design
----------

The proposed HAL design consists of several parts:

![HAL Struct](hal_struct.png)



HAL API
-------

OpenCV provides HAL interface description (as a C header file `hal_interface.h`).
This header will be used both in OpenCV and in HAL implementations.

HAL API is a set of functions prototypes.
All funcions has plain C interface (no classes, no stl, no exceptions, etc.).

```C
// This two functions must be implemented in all HALs.

CV_HAL_API CvHalStatus cvhal_init(CvHalContext* context); // initialize HAL
CV_HAL_API const char* cvhal_info(); // Get information about HAL (name, vendor, version, etc.)

// This is the set of HAL functions.

CV_HAL_API CvHalStatus cvhal_hamming_dist(unsigned char* a, unsigned char* b, size_t len, int* result, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_resize(CvHalMat* src, CvHalMat* dst, int interpolation, CvHalContext* context);

CV_HAL_API CvHalStatus cvhal_erode(CvHalMat* src, CvHalMat* dst, unsigned char* kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext* context);
```

This interface can only be extended in new OpenCV versions, we can add new functions, but we can't change existed prototypes.

A HAL doesn't have to implement all operations. It can implement only a subset of this interface.
Also HAL doesn't have to support all input parameters (for example, interpolation mode in resize).
Each function can return `CV_HAL_NOT_IMPLEMENTED` error code for unsupported parameters.
In this case OpenCV will use default implementation.

The HAL API doesn't depend on OpenCV API. It has own types, macroses and constants definitions (like `CvHalMat`).

In addition to input parameters all HAL functions accept `CvHalContext` struct, which contains some additional information like:
* OpenCV version
* Some parameters (number of threads)
* Information about CUDA device
* CUDA stream id

```C
typedef struct _CvHalContext {
    int opencv_version;

    // OpenCV 3.0
    int num_threads;

    // OpenCV 3.1
    cudaStream_t cuda_stream;
} CvHalContext;
```

HAL API provides set of macroses for reading this parameters.
We can add new entries to context in new OpenCV versions, so this macroses checks current OpenCV version and returns default values for old releases.

```C
#define CV_HAL_GET_OPENCV_VERSION(context) \
    ((context) ? context->opencv_version : 0)

// OpenCV 3.0

#define CV_HAL_GET_NUM_THREADS(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 300 ? context->num_threads : -1)

// OpenCV 3.1

#define CV_HAL_GET_CUDA_STREAM(context) \
    (CV_HAL_GET_OPENCV_VERSION(context) >= 310 ? context->cuda_stream : (cudaStream_t) 0)
```



HAL implementation
------------------

A HAL implementation provides a subset of function from HAL API.


The HAL implementation provides own header file - `hal_impl.h`.
This header is used for static build.

The header defines macros, describing which functions are provided by the HAL:

```C
//#define CV_HAL_HAS_HAMMING_DIST
#define CV_HAL_HAS_RESIZE
#define CV_HAL_HAS_ERODE
```

Also this header can contain some inline functions, which will be used only in static mode:

```C
#define CV_HAL_HAS_ROUND
CV_HAL_INLINE int cvhal_round(double val)
{
    return ...;
}
```

OpenCV provides a template for this header - `hal_impl_templ.h`.



HAL Layer
---------

HAL Layer is a thin С++ wrapper for HAL API (actually, it is a part of `core` module).
It is used to hide differences between modes and provide simple API for OpenCV modules.
It is a private interface and it is used only inside OpenCV.
It is designed to be as small as possible to avoid overhead from this layer.
This wrappers are generated automatically from `hal_interface.h` header by scripts.

For each function in HAL API it provides an inline C++ wrapper, that calls corresponding function from HAL implementation.
All this functions return boolean value, which indicates if HAL call was successfull or not.
Also this wrapper-function converts OpenCV structures (`Mat`, `vector`, etc.) to HAL types and fill `CvHalContext` struct.

If OpenCV was built without HAL support, all wrapper-functions return `false`.
If OpenCV was built with statically-linked HAL and the HAL doesn't provide the implementation, the wrapper-function also returns `false`, otherwise it calls HAL implementation.
If OpenCV was built with dynamically-loadable HAL support, the wrapper-function tries to load implementation at runtime.

```C
static inline bool resize(const Mat & src, const Mat & dst, int interpolation)
{
    (void) src;
    (void) dst;
    (void) interpolation;

#if (defined CV_HAL_STATIC && !defined CV_HAL_HAS_RESIZE) || (!defined CV_HAL_STATIC && !defined CV_HAL_DYNAMIC)
    return false;
#else
    #if defined CV_HAL_DYNAMIC
        if (!detail::isInitialized)
            detail::initPointers();

        if (!detail::cvhal_resize_func_ptr)
            return false;
    #endif

    CvHalStatus status;

    CvHalMat hal_src = detail::toCvHalMat(src);
    CvHalMat hal_dst = detail::toCvHalMat(dst);
    CvHalContext context = detail::getContext();

    #if defined CV_HAL_STATIC
        status = cvhal_resize(&hal_src, &hal_dst, interpolation, &context);
    #else
        status = detail::cvhal_resize_func_ptr(&hal_src, &hal_dst, interpolation, &context);
    #endif

    return status == CV_HAL_SUCCESS;
#endif
}
```

Also the HAL Layer contains wrappers for inline functions from HAL implementation.
This wrappers are used only for static build.

```C
static inline int round(double val)
{
#if defined CV_HAL_HAS_ROUND
    return cvhal_round(val);
#else
    // default implementation
    return cvRound(val);
#endif
}
```

 

OpenCV modules
--------------

All OpenCV modules work with HAL via HAL Layer.
First OpenCV function tries to call HAL implemetation and, if HAL fails, OpenCV uses own implementation.

```C
void cv::resize(const Mat& src, Mat& dst, int interpolation)
{
    // First try to call HAL
    if (hal::resize(src, dst, interpolation))
        return;

    // If it fails use built-in implementation
    ...
}
```



OpenCV user
-----------

OpenCV user can get information about HAL:

```C
CV_EXPORTS String getHalInfo();
```

OpenCV user can load own HAL in dynamic mode:

```C
CV_EXPORTS void loadHalImpl(const String& halLibName);
```
