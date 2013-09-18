#include "utility.hpp"

#include <sstream>
#include <stdexcept>

#if defined(OPENCV_OS_FAMILY_UNIX)

//
// OPENCV_OS_FAMILY_UNIX
//

#include <dlfcn.h>

class cv::SharedLibrary::Impl
{
public:
    Impl();

    bool loadImpl(const std::string& path);
    void unloadImpl();
    bool isLoadedImpl() const;

    void* findSymbolImpl(const std::string& name);

    static std::string suffixImpl();

private:
    void* handle_;
    static cv::Mutex mutex_;
};

cv::Mutex cv::SharedLibrary::Impl::mutex_;

inline cv::SharedLibrary::Impl::Impl()
{
    handle_ = 0;
}

inline bool cv::SharedLibrary::Impl::loadImpl(const std::string& path)
{
    Mutex::ScopedLock lock(mutex_);
    handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    return handle_ != 0;
}

inline void cv::SharedLibrary::Impl::unloadImpl()
{
    if (handle_)
    {
        Mutex::ScopedLock lock(mutex_);
        dlclose(handle_);
        handle_ = 0;
    }
}

inline bool cv::SharedLibrary::Impl::isLoadedImpl() const
{
    return handle_ != 0;
}

inline void* cv::SharedLibrary::Impl::findSymbolImpl(const std::string& name)
{
    Mutex::ScopedLock lock(mutex_);
    return dlsym(handle_, name.c_str());
}

inline std::string cv::SharedLibrary::Impl::suffixImpl()
{
#if defined(__APPLE__)
    return ".dylib";
#elif defined(__CYGWIN__)
    return ".dll";
#else
    return ".so";
#endif
}

#elif defined(OPENCV_OS_FAMILY_WINDOWS)

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

class cv::SharedLibrary::Impl
{
public:
    Impl();

    bool loadImpl(const std::string& path);
    void unloadImpl();
    bool isLoadedImpl() const;

    void* findSymbolImpl(const std::string& name);

    static std::string suffixImpl();

private:
    HMODULE handle_;
    static cv::Mutex mutex_;
};

cv::Mutex cv::SharedLibrary::Impl::mutex_;

inline cv::SharedLibrary::Impl::Impl()
{
    handle_ = 0;
}

inline bool cv::SharedLibrary::Impl::loadImpl(const std::string& path)
{
    Mutex::ScopedLock lock(mutex_);
    UINT oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    handle_ = LoadLibraryExA(path.c_str(), 0, 0);
    SetErrorMode(oldErrorMode);
    return handle_ != 0;
}

inline void cv::SharedLibrary::Impl::unloadImpl()
{
    if (handle_)
    {
        Mutex::ScopedLock lock(mutex_);
        FreeLibrary(handle_);
        handle_ = 0;
    }
}

inline bool cv::SharedLibrary::Impl::isLoadedImpl() const
{
    return handle_ != 0;
}

inline void* cv::SharedLibrary::Impl::findSymbolImpl(const std::string& name)
{
    Mutex::ScopedLock lock(mutex_);
    return (void*) GetProcAddress(handle_, name.c_str());
}

inline std::string cv::SharedLibrary::Impl::suffixImpl()
{
    return ".dll";
}

#endif

cv::SharedLibrary::SharedLibrary()
{
    impl_ = new Impl;
}

cv::SharedLibrary::SharedLibrary(const std::string& path)
{
    impl_ = new Impl;
    load(path);
}

cv::SharedLibrary::~SharedLibrary()
{
    delete impl_;
}

void cv::SharedLibrary::load(const std::string& path)
{
    if (isLoaded())
        throw std::runtime_error("Library is already loaded");

    const bool res = impl_->loadImpl(path);

    if (!res)
    {
        std::ostringstream msg;
        msg << "Can't open library " << path;
        throw std::runtime_error(msg.str());
    }
}

void cv::SharedLibrary::unload()
{
    if (isLoaded())
        impl_->unloadImpl();
}

bool cv::SharedLibrary::isLoaded() const
{
    return impl_->isLoadedImpl();
}

bool cv::SharedLibrary::hasSymbol(const std::string& name)
{
    if (!isLoaded())
        throw std::runtime_error("Library is not loaded");

    return impl_->findSymbolImpl(name) != 0;
}

void* cv::SharedLibrary::getSymbol(const std::string& name)
{
    if (!isLoaded())
        throw std::runtime_error("Library is not loaded");

    void* result = impl_->findSymbolImpl(name);

    if (result)
        return result;
    else
    {
        std::ostringstream msg;
        msg << "Symbol " << name << " not found";
        throw std::runtime_error(msg.str());
    }
}

std::string cv::SharedLibrary::suffix()
{
    return Impl::suffixImpl();
}
