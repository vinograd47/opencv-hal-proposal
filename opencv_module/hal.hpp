// This is generated file. Do not edit it.

#ifndef __OPENCV_HAL_HPP__
#define __OPENCV_HAL_HPP__

#if !defined __OPENCV_BUILD
#   error "This is a private header"
#endif

#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>

// OpenCV supports both dynamically-loadable and statically-linked HALs.

#if defined CV_HAL_STATIC
#   include "hal_impl.h"
#elif defined CV_HAL_DYNAMIC
#   include "hal_interface.h"
#endif

// C++ wrappers for HAL API

namespace cv { namespace hal {

// Static mode calls HAL functions directly.
// Dynamic mode calls HAL functions through pointers.

namespace detail {
    #if defined CV_HAL_DYNAMIC
        CV_EXPORTS extern bool isInitialized;
        CV_EXPORTS void initPointers();

        typedef CvHalStatus (*cvhal_hamming_dist_func_ptr_t)(unsigned char * a, unsigned char * b, size_t len, int * result, CvHalContext * context);
        extern cvhal_hamming_dist_func_ptr_t cvhal_hamming_dist_func_ptr;

        typedef CvHalStatus (*cvhal_resize_func_ptr_t)(CvHalMat * src, CvHalMat * dst, int interpolation, CvHalContext * context);
        extern cvhal_resize_func_ptr_t cvhal_resize_func_ptr;

        typedef CvHalStatus (*cvhal_erode_func_ptr_t)(CvHalMat * src, CvHalMat * dst, unsigned char * kernel, CvHalSize kernelSize, CvHalPoint anchor, CvHalContext * context);
        extern cvhal_erode_func_ptr_t cvhal_erode_func_ptr;
    #endif

    #if defined CV_HAL_STATIC || defined CV_HAL_DYNAMIC
        static inline CvHalMat toCvHalMat(const Mat& mat)
        {
            CvHalMat hal_mat;
            hal_mat.data = mat.data;
            hal_mat.step = mat.step;
            hal_mat.rows = mat.rows;
            hal_mat.cols = mat.cols;
            hal_mat.depth = mat.depth();
            hal_mat.channels = mat.channels();
            hal_mat.datastart = mat.datastart;
            Size wholeSize;
            Point ofs;
            mat.locateROI(wholeSize, ofs);
            hal_mat.xoff = ofs.x;
            hal_mat.yoff = ofs.y;
            return hal_mat;
        }

        static inline CvHalContext getContext()
        {
            CvHalContext context;

            context.opencv_version = CV_VERSION_EPOCH * 100 + CV_VERSION_MAJOR * 10 + CV_VERSION_MINOR;

            // OpenCV 3.0
            context.num_threads = getNumThreads();

            // OpenCV 3.1
            context.cuda_stream = NULL;

            return context;
        }
    #endif
} // namespace detail

// All functions return true, if HAL call was successfull
// and false otherwise (for example, when HAL doesn't implement the functions).
// If HAL fails OpenCV uses own implementation.

static inline bool hamming_dist(unsigned char * a, unsigned char * b, size_t len, int * result)
{
    (void) a;
    (void) b;
    (void) len;
    (void) result;

#if (defined CV_HAL_STATIC && !defined CV_HAL_HAS_HAMMING_DIST) || (!defined CV_HAL_STATIC && !defined CV_HAL_DYNAMIC)
    return false;
#else
    #if defined CV_HAL_DYNAMIC
        if (!detail::isInitialized)
            detail::initPointers();

        if (!detail::cvhal_hamming_dist_func_ptr)
            return false;
    #endif

    CvHalStatus status;

    CvHalContext context = detail::getContext();

    #if defined CV_HAL_STATIC
        status = cvhal_hamming_dist(a, b, len, result, &context);
    #else
        status = detail::cvhal_hamming_dist_func_ptr(a, b, len, result, &context);
    #endif

    return status == CV_HAL_SUCCESS;
#endif
}

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

static inline bool erode(const Mat & src, const Mat & dst, unsigned char * kernel, Size kernelSize, Point anchor)
{
    (void) src;
    (void) dst;
    (void) kernel;
    (void) kernelSize;
    (void) anchor;

#if (defined CV_HAL_STATIC && !defined CV_HAL_HAS_ERODE) || (!defined CV_HAL_STATIC && !defined CV_HAL_DYNAMIC)
    return false;
#else
    #if defined CV_HAL_DYNAMIC
        if (!detail::isInitialized)
            detail::initPointers();

        if (!detail::cvhal_erode_func_ptr)
            return false;
    #endif

    CvHalStatus status;

    CvHalMat hal_src = detail::toCvHalMat(src);
    CvHalMat hal_dst = detail::toCvHalMat(dst);
    CvHalContext context = detail::getContext();

    #if defined CV_HAL_STATIC
        status = cvhal_erode(&hal_src, &hal_dst, kernel, &kernelSize.width, &anchor.x, &context);
    #else
        status = detail::cvhal_erode_func_ptr(&hal_src, &hal_dst, kernel, &kernelSize.width, &anchor.x, &context);
    #endif

    return status == CV_HAL_SUCCESS;
#endif
}

// Functions that are available only in static mode

static inline int round(double val)
{
#if defined CV_HAL_HAS_ROUND
    return cvhal_round(val);
#else
    // default implementation
    std::cout << "built-in round" << std::endl;
    return cvRound(val);
#endif
}

}} // namespace cv { namespace hal {

#endif
