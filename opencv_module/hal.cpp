// This is generated file. Do not edit it.

#include <iostream>

#if defined CV_HAL_DYNAMIC
#   include <pthread.h>
#   include "shared_library.hpp"
#endif

#include "hal.hpp"
#include "opencv_module.hpp"

#if defined CV_HAL_STATIC

namespace
{
    // Implicit initialization

    class StaticHalInitializer
    {
    public:
        StaticHalInitializer()
        {
            std::cout << "Initialize static HAL \n" << std::endl;

            CvHalStatus status = cvhal_init();
            if (status != CV_HAL_SUCCESS)
            {
                std::cout << "cvhal_init failed \n" << std::endl;
                return;
            }
        }
    };

    StaticHalInitializer initializer;
}

void cv::loadHalImpl(const String&)
{
    std::cout << "OpenCV was built with static HAL" << std::endl;
}

cv::String cv::getHalInfo()
{
    return cvhal_info();
}

#elif defined CV_HAL_DYNAMIC

// Pointers for dynamic mode

namespace cv { namespace hal { namespace detail {

bool isInitialized = false;
cv::SharedLibrary halLib;
cv::Mutex mutex;

typedef CvHalStatus (*cvhal_init_func_ptr_t)();
typedef const char* (*cvhal_info_func_ptr_t)();

cvhal_init_func_ptr_t cvhal_init_func_ptr = NULL;
cvhal_info_func_ptr_t cvhal_info_func_ptr = NULL;

cvhal_hamming_dist_func_ptr_t cvhal_hamming_dist_func_ptr = NULL;
cvhal_resize_func_ptr_t cvhal_resize_func_ptr = NULL;
cvhal_erode_func_ptr_t cvhal_erode_func_ptr = NULL;

}}}

void cv::loadHalImpl(const String& halLibName)
{
    using namespace cv::hal::detail;

    cv::AutoLock lock(mutex);

    // unload previous HAL
    if (halLib.isLoaded())
        halLib.unload();

    cvhal_hamming_dist_func_ptr = NULL;
    cvhal_resize_func_ptr = NULL;
    cvhal_erode_func_ptr = NULL;

    // load new HAL

    std::cout << "Initialize shared HAL : " << halLibName << "\n" << std::endl;

    halLib.load(halLibName);
    if (!halLib.isLoaded())
    {
        std::cout << "Can't load " << halLibName << " library \n" << std::endl;
        isInitialized = true;
        return;
    }

    cvhal_init_func_ptr = (cvhal_init_func_ptr_t) halLib.getSymbol("cvhal_init");
    if (!cvhal_init_func_ptr)
    {
        std::cout << halLibName << " doesn't have cvhal_init function \n" << std::endl;
        isInitialized = true;
        return;
    }

    CvHalStatus status = cvhal_init_func_ptr();
    if (status != CV_HAL_SUCCESS)
    {
        std::cout << "cvhal_init failed \n" << std::endl;
        isInitialized = true;
        return;
    }

    cvhal_info_func_ptr = (cvhal_info_func_ptr_t) halLib.getSymbol("cvhal_info");
    if (!cvhal_info_func_ptr)
    {
        std::cout << halLibName << " doesn't have cvhal_info function \n" << std::endl;
        isInitialized = true;
        return;
    }

    // HAL module was loaded correctly
    // load all available functions

    cvhal_hamming_dist_func_ptr = (cvhal_hamming_dist_func_ptr_t) halLib.getSymbol("cvhal_hamming_dist");
    cvhal_resize_func_ptr = (cvhal_resize_func_ptr_t) halLib.getSymbol("cvhal_resize");
    cvhal_erode_func_ptr = (cvhal_erode_func_ptr_t) halLib.getSymbol("cvhal_erode");

    isInitialized = true;
}

namespace
{
    pthread_once_t once_control = PTHREAD_ONCE_INIT;

    void init_routine()
    {
        // Read HAL name from enviroment variable.
        // It can be done it different ways (scan some directory,
        // config file, etc.).

        const char* halLibName = getenv("OPENCV_HAL_MODULE");
        if (!halLibName)
        {
            std::cout << "OPENCV_HAL_MODULE env variable is empty \n" << std::endl;
            cv::hal::detail::isInitialized = true;
            return;
        }

        cv::loadHalImpl(halLibName);
    }
}

void cv::hal::detail::initPointers()
{
    pthread_once(&once_control, init_routine);
}

cv::String cv::getHalInfo()
{
    using namespace cv::hal::detail;

    if (!isInitialized)
        initPointers();

    if (!cvhal_info_func_ptr)
        return "No HAL";

    return cvhal_info_func_ptr();
}

#else

void cv::loadHalImpl(const String&)
{
    std::cout << "OpenCV was built without HAL support" << std::endl;
}

cv::String cv::getHalInfo()
{
    return "No HAL";
}

#endif
