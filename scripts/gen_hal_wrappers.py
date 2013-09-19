#!/usr/bin/python
import sys
from string import Template
import CppHeaderParser



hal_impl_templ_h_file_templ = Template(\
R"""// This is generated file. Do not edit it.

#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include "hal_interface.h"

${macros_list}

// This functions can be used only in static build as inlined

//#define CV_HAL_HAS_ROUND
//CV_HAL_INLINE int cvhal_round(double val)
//{
//    // implementation
//}

#endif
""")



hal_hpp_file_templ = Template(\
R"""// This is generated file. Do not edit it.

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

${funcs_ptr_decl_list}
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

        struct HalContext
        {
            void* data[CV_HAL_CONTEXT_SIZE];
        };

        static inline HalContext getContext()
        {
            int opencv_version = CV_VERSION_EPOCH * 100 + CV_VERSION_MAJOR * 10 + CV_VERSION_MINOR;
            int numThreads = getNumThreads();

            HalContext context =
            {
                reinterpret_cast<void*>(opencv_version),

                // OpenCV 3.0
                reinterpret_cast<void*>(numThreads)
            };

            return context;
        }
    #endif
} // namespace detail

// All functions return true, if HAL call was successfull
// and false otherwise (for example, when HAL doesn't implement the functions).
// If HAL fails OpenCV uses own implementation.

${funcs_wrap_impl_list}

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
""")



func_ptr_decl_templ = Template(\
R"""        typedef CvHalStatus (*cvhal_${func_name}_func_ptr_t)(${param_hal_decl_list});
        extern cvhal_${func_name}_func_ptr_t cvhal_${func_name}_func_ptr;""")



func_wrap_code_templ = Template(\
R"""static inline bool ${func_name}(${param_ocv_decl_list})
{
${param_unused_list}

#if (defined CV_HAL_STATIC && !defined CV_HAL_HAS_${func_name_upper}) || (!defined CV_HAL_STATIC && !defined CV_HAL_DYNAMIC)
    return false;
#else
    #if defined CV_HAL_DYNAMIC
        if (!detail::isInitialized)
            detail::initPointers();

        if (!detail::cvhal_${func_name}_func_ptr)
            return false;
    #endif

    CvHalStatus status;

${param_conversion_list}

    #if defined CV_HAL_STATIC
        status = cvhal_${func_name}(${param_pass_list});
    #else
        status = detail::cvhal_${func_name}_func_ptr(${param_pass_list});
    #endif

    return status == CV_HAL_SUCCESS;
#endif
}""")



hal_cpp_file_templ = Template(\
R"""// This is generated file. Do not edit it.

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

            HalContext context = getContext();

            CvHalStatus status = cvhal_init(context.data);
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

typedef CvHalStatus (*cvhal_init_func_ptr_t)(CvHalContext context);
typedef const char* (*cvhal_info_func_ptr_t)();

cvhal_init_func_ptr_t cvhal_init_func_ptr = NULL;
cvhal_info_func_ptr_t cvhal_info_func_ptr = NULL;

${funcs_ptr_def_list}

}}}

void cv::loadHalImpl(const String& halLibName)
{
    using namespace cv::hal::detail;

    cv::AutoLock lock(mutex);

    // unload previous HAL
    if (halLib.isLoaded())
        halLib.unload();

${funcs_ptr_clear_list}

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

    HalContext context = getContext();

    CvHalStatus status = cvhal_init_func_ptr(context.data);
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

${funcs_load_list}

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
""")



class ParamInfo(object):
    def __init__(self, paramName, paramType):
        self.name = paramName
        self.halType = paramType
        if paramType == 'CvHalMat *':
            self.ocvType = 'const Mat &'
        elif paramType == 'CvHalSize':
            self.ocvType = 'Size'
        elif paramType == 'CvHalPoint':
            self.ocvType = 'Point'
        else:
            self.ocvType = paramType

    def gen_hal_decl(self):
        return self.halType + ' ' + self.name

    def gen_ocv_decl(self):
        if self.halType == 'CvHalContext':
            return ''
        else:
            return self.ocvType + ' ' + self.name

    def gen_unused(self):
        if self.halType == 'CvHalContext':
            return ''
        else:
            return '    (void) ' + self.name + ';'


    def gen_conversion(self):
        if self.halType == 'CvHalMat *':
            return Template('    CvHalMat hal_${param_name} = detail::toCvHalMat(${param_name});').substitute(param_name=self.name)
        elif self.halType == 'CvHalContext':
            return '    detail::HalContext context = detail::getContext();'
        else:
            return ''

    def gen_pass(self):
        if self.halType == 'CvHalMat *':
            return '&hal_' + self.name
        elif self.halType == 'CvHalContext':
            return 'context.data'
        elif self.halType == 'CvHalSize':
            return '&' + self.name + '.width'
        elif self.halType == 'CvHalPoint':
            return '&' + self.name + '.x'
        else:
            return self.name



class FuncInfo(object):
    def __init__(self, name, params):
        self.fullName = name
        self.shortName = name[6:]
        self.params = params

    def gen_macros(self):
        return '//#define CV_HAL_HAS_' + self.shortName.upper()

    def gen_ptr_decl(self):
        param_hal_decl_list = []
        for param in self.params:
            param_hal_decl_list.append(param.gen_hal_decl())
        return func_ptr_decl_templ.substitute(func_name=self.shortName, param_hal_decl_list=', '.join(param_hal_decl_list))

    def gen_wrap_impl(self):
        param_ocv_decl_list = []
        param_unused_list = []
        param_conversion_list = []
        param_pass_list = []
        for param in self.params:
            ocv_decl = param.gen_ocv_decl()
            if ocv_decl != '':
                param_ocv_decl_list.append(ocv_decl)
            unused = param.gen_unused()
            if unused != '':
                param_unused_list.append(unused)
            conversion = param.gen_conversion()
            if conversion != '':
                param_conversion_list.append(conversion)
            param_pass_list.append(param.gen_pass())
        return func_wrap_code_templ.substitute(func_name=self.shortName, func_name_upper=self.shortName.upper(),
                                               param_ocv_decl_list=', '.join(param_ocv_decl_list),
                                               param_unused_list='\n'.join(param_unused_list),
                                               param_conversion_list='\n'.join(param_conversion_list),
                                               param_pass_list=', '.join(param_pass_list))

    def gen_ptr_def(self):
        return Template('${func_name}_func_ptr_t ${func_name}_func_ptr = NULL;').substitute(func_name=self.fullName)

    def gen_ptr_clear(self):
        return Template('    ${func_name}_func_ptr = NULL;').substitute(func_name=self.fullName)

    def gen_load(self):
        return Template('    ${func_name}_func_ptr = (${func_name}_func_ptr_t) halLib.getSymbol("${func_name}");').substitute(func_name=self.fullName)



class HalWrapperGenerator(object):
    def __init__(self):
        self.clear()

    def clear(self):
        self.funcs = []

    def parse_funcs_list(self, funcs_list):
        for func in funcs_list:
            if func['rtnType'] != 'CV_HAL_API CvHalStatus':
                continue
            funcName = func['name']
            if not funcName.startswith('cvhal_'):
                continue
            if funcName == 'cvhal_init' or funcName == 'cvhal_info':
                continue
            funcParams = []
            for param in func['parameters']:
                funcParams.append(ParamInfo(param['name'], param['type']))
            self.funcs.append(FuncInfo(funcName, funcParams))

    def gen_hal_impl_templ_h_file(self):
        macros_list = []
        for func in self.funcs:
            macros_list.append(func.gen_macros())
        return hal_impl_templ_h_file_templ.substitute(macros_list='\n'.join(macros_list))

    def gen_hal_hpp_file(self):
        funcs_ptr_decl_list = []
        funcs_wrap_impl_list = []
        for func in self.funcs:
            funcs_ptr_decl_list.append(func.gen_ptr_decl())
            funcs_wrap_impl_list.append(func.gen_wrap_impl())
        return hal_hpp_file_templ.substitute(funcs_ptr_decl_list='\n\n'.join(funcs_ptr_decl_list),
                                             funcs_wrap_impl_list='\n\n'.join(funcs_wrap_impl_list))

    def gen_hal_cpp_file(self):
        funcs_ptr_def_list = []
        funcs_ptr_clear_list = []
        funcs_load_list = []
        for func in self.funcs:
            funcs_ptr_def_list.append(func.gen_ptr_def())
            funcs_ptr_clear_list.append(func.gen_ptr_clear())
            funcs_load_list.append(func.gen_load())
        return hal_cpp_file_templ.substitute(funcs_ptr_def_list='\n'.join(funcs_ptr_def_list),
                                             funcs_ptr_clear_list='\n'.join(funcs_ptr_clear_list),
                                             funcs_load_list='\n'.join(funcs_load_list))

    def gen(self, hal_interface_h_file_name, hal_impl_templ_h_file_name, hal_hpp_file_name, hal_cpp_file_name):
        self.clear()

        halImplHeader = CppHeaderParser.CppHeader(hal_interface_h_file_name)
        self.parse_funcs_list(halImplHeader.functions)

        hal_impl_templ_h_file = open(hal_impl_templ_h_file_name, 'w')
        hal_impl_templ_h_file.write(self.gen_hal_impl_templ_h_file())
        hal_impl_templ_h_file.close()

        hal_hpp_file = open(hal_hpp_file_name, 'w')
        hal_hpp_file.write(self.gen_hal_hpp_file())
        hal_hpp_file.close()

        hal_cpp_file = open(hal_cpp_file_name, 'w')
        hal_cpp_file.write(self.gen_hal_cpp_file())
        hal_cpp_file.close()



if __name__ == '__main__':
    hal_interface_h_file_name = 'hal_interface.h'
    hal_impl_templ_h_file_name = 'hal_impl_templ.h'
    hal_hpp_file_name = 'hal.hpp'
    hal_cpp_file_name = 'hal.cpp'
    if len(sys.argv) > 1:
        hal_interface_h_file_name = sys.argv[1]
    if len(sys.argv) > 2:
        hal_impl_templ_h_file_name = sys.argv[2]
    if len(sys.argv) > 3:
        hal_hpp_file_name = sys.argv[3]
    if len(sys.argv) > 4:
        hal_cpp_file_name = sys.argv[4]

    generator = HalWrapperGenerator()
    generator.gen(hal_interface_h_file_name, hal_impl_templ_h_file_name, hal_hpp_file_name, hal_cpp_file_name)
