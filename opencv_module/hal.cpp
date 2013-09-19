// This is generated file. Do not edit it.

#include <iostream>

#if defined CV_HAL_DYNAMIC
#   if (defined WIN32 || defined _WIN32) && !defined __CYGWIN__
#       define WIN32_LEAN_AND_MEAN
#       define NOMINMAX
#       include <windows.h>
#   else
#       include <pthread.h>
#   endif
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
            using namespace cv::hal::detail;

            std::cout << "Initialize static HAL \n" << std::endl;

            CvHalContext context = getContext();

            CvHalStatus status = cvhal_init(&context);
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

typedef CvHalStatus (*cvhal_init_func_ptr_t)(CvHalContext * context);
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

    CvHalContext context = getContext();

    CvHalStatus status = cvhal_init_func_ptr(&context);
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
    void initPointersImpl()
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
#if (defined WIN32 || defined _WIN32) && !defined __CYGWIN__
    static volatile int initialized = 0;
    static HANDLE mtx;

    if (!initialized)
    {
        if (!mtx)
        {
            HANDLE mymtx;
            mymtx = CreateMutex(NULL, 0, NULL);
            if (InterlockedCompareExchangePointer(&mtx, mymtx, NULL) != NULL)
                CloseHandle(mymtx);
        }

        WaitForSingleObject(mtx);

        if (!initialized)
        {
            initPointersImpl();
            initialized = 1;
        }

        ReleaseMutex(mtx);
    }
#else
    static pthread_once_t once_control = PTHREAD_ONCE_INIT;
    pthread_once(&once_control, initPointersImpl);
#endif
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
