// This is generated file. Do not edit it.

#include "hal.hpp"
#include <iostream>

#if defined(CV_HAL_DYNAMIC)
#   include <dlfcn.h>
#   include <pthread.h>
#endif

#if defined(CV_HAL_STATIC)

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
                std::cerr << "cvhal_init failed \n" << std::endl;
                return;
            }
        }
    };

    StaticHalInitializer initializer;
}

void cv::loadHalImpl(const std::string&)
{
    std::cerr << "OpenCV was built with static HAL" << std::endl;
}

std::string cv::getHalInfo()
{
    return cvhal_info();
}

#elif defined(CV_HAL_DYNAMIC)

// Pointers for dynamic mode

namespace cv { namespace hal { namespace detail {

bool isInitialized = false;
void* halLib = NULL;

typedef CvHalStatus (*cvhal_init_func_ptr_t)();
typedef const char* (*cvhal_info_func_ptr_t)();

cvhal_init_func_ptr_t cvhal_init_func_ptr = NULL;
cvhal_info_func_ptr_t cvhal_info_func_ptr = NULL;

cvhal_hamming_dist_func_ptr_t cvhal_hamming_dist_func_ptr = NULL;
cvhal_resize_func_ptr_t cvhal_resize_func_ptr = NULL;
cvhal_erode_func_ptr_t cvhal_erode_func_ptr = NULL;

}}}

void cv::loadHalImpl(const std::string& halLibName)
{
    using namespace cv::hal::detail;

    // unload previous HAL
    if (halLib)
    {
        dlclose(halLib);
        halLib = NULL;
    }

    cvhal_hamming_dist_func_ptr = NULL;
    cvhal_resize_func_ptr = NULL;
    cvhal_erode_func_ptr = NULL;

    // load new HAL

    std::cout << "Initialize shared HAL : " << halLibName << "\n" << std::endl;

    halLib = dlopen(halLibName.c_str(), RTLD_NOW);
    if (!halLib)
    {
        const char* msg = dlerror();
        std::cerr << "Can't load " << halLibName << " library :" << (msg ? msg : "") << "\n" << std::endl;
        isInitialized = true;
        return;
    }

    cvhal_init_func_ptr = (cvhal_init_func_ptr_t) dlsym(halLib, "cvhal_init");
    if (!cvhal_init_func_ptr)
    {
        std::cerr << halLibName << " doesn't have cvhal_init function \n" << std::endl;
        isInitialized = true;
        return;
    }

    CvHalStatus status = cvhal_init_func_ptr();
    if (status != CV_HAL_SUCCESS)
    {
        std::cerr << "cvhal_init failed \n" << std::endl;
        isInitialized = true;
        return;
    }

    cvhal_info_func_ptr = (cvhal_info_func_ptr_t) dlsym(halLib, "cvhal_info");
    if (!cvhal_info_func_ptr)
    {
        std::cerr << halLibName << " doesn't have cvhal_info function \n" << std::endl;
        isInitialized = true;
        return;
    }

    // HAL module was loaded correctly
    // load all available functions

    cvhal_hamming_dist_func_ptr = (cvhal_hamming_dist_func_ptr_t) dlsym(halLib, "cvhal_hamming_dist");
    cvhal_resize_func_ptr = (cvhal_resize_func_ptr_t) dlsym(halLib, "cvhal_resize");
    cvhal_erode_func_ptr = (cvhal_erode_func_ptr_t) dlsym(halLib, "cvhal_erode");

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
            std::cerr << "OPENCV_HAL_MODULE env variable is empty \n" << std::endl;
            cv::hal::detail::isInitialized = true;
            return;
        }

        cv::loadHalImpl(halLibName);
    }
}

void cv::hal::detail::initHalPointers()
{
    pthread_once(&once_control, init_routine);
}

std::string cv::getHalInfo()
{
    using namespace cv::hal::detail;

    if (!isInitialized)
        initHalPointers();

    if (!cvhal_info_func_ptr)
        return "No HAL";

    return cvhal_info_func_ptr();
}

#else

void cv::loadHalImpl(const std::string&)
{
    std::cerr << "OpenCV was built without HAL support" << std::endl;
}

std::string cv::getHalInfo()
{
    return "No HAL";
}

#endif