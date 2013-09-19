#include <opencv2/core/utility.hpp>
#include "shared_library.hpp"

#if (defined WIN32 || defined _WIN32) && !defined __CYGWIN__

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>

class cv::SharedLibrary::Impl
{
public:
    Impl();

    bool load(const cv::String& path);
    void unload();
    bool isLoaded() const;

    void* findSymbol(const cv::String& name);

    static cv::String suffix();

private:
    HMODULE handle_;
    static cv::Mutex mutex_;
};

cv::Mutex cv::SharedLibrary::Impl::mutex_;

inline cv::SharedLibrary::Impl::Impl()
{
    handle_ = 0;
}

inline bool cv::SharedLibrary::Impl::load(const String& path)
{
    AutoLock lock(mutex_);
    UINT oldErrorMode = SetErrorMode(SEM_FAILCRITICALERRORS);
    handle_ = LoadLibraryExA(path.c_str(), 0, 0);
    SetErrorMode(oldErrorMode);
    return handle_ != 0;
}

inline void cv::SharedLibrary::Impl::unload()
{
    if (handle_)
    {
        AutoLock lock(mutex_);
        FreeLibrary(handle_);
        handle_ = 0;
    }
}

inline bool cv::SharedLibrary::Impl::isLoaded() const
{
    return handle_ != 0;
}

inline void* cv::SharedLibrary::Impl::findSymbol(const String& name)
{
    AutoLock lock(mutex_);
    return (void*) GetProcAddress(handle_, name.c_str());
}

inline cv::String cv::SharedLibrary::Impl::suffix()
{
    return ".dll";
}

#else

#include <dlfcn.h>

class cv::SharedLibrary::Impl
{
public:
    Impl();

    bool load(const cv::String& path);
    void unload();
    bool isLoaded() const;

    void* findSymbol(const cv::String& name);

    static cv::String suffix();

private:
    void* handle_;
    static cv::Mutex mutex_;
};

cv::Mutex cv::SharedLibrary::Impl::mutex_;

inline cv::SharedLibrary::Impl::Impl()
{
    handle_ = 0;
}

inline bool cv::SharedLibrary::Impl::load(const String& path)
{
    AutoLock lock(mutex_);
    handle_ = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    return handle_ != 0;
}

inline void cv::SharedLibrary::Impl::unload()
{
    if (handle_)
    {
        AutoLock lock(mutex_);
        dlclose(handle_);
        handle_ = 0;
    }
}

inline bool cv::SharedLibrary::Impl::isLoaded() const
{
    return handle_ != 0;
}

inline void* cv::SharedLibrary::Impl::findSymbol(const String& name)
{
    AutoLock lock(mutex_);
    return dlsym(handle_, name.c_str());
}

inline cv::String cv::SharedLibrary::Impl::suffix()
{
#if defined(__APPLE__)
    return ".dylib";
#elif defined(__CYGWIN__)
    return ".dll";
#else
    return ".so";
#endif
}

#endif

cv::SharedLibrary::SharedLibrary()
{
    impl_ = makePtr<Impl>();
}

cv::SharedLibrary::SharedLibrary(const String& path)
{
    impl_ = makePtr<Impl>();
    load(path);
}

bool cv::SharedLibrary::load(const String& path)
{
    if (isLoaded())
        return false;

    return impl_->load(path);
}

void cv::SharedLibrary::unload()
{
    if (isLoaded())
        impl_->unload();
}

bool cv::SharedLibrary::isLoaded() const
{
    return impl_->isLoaded();
}

void* cv::SharedLibrary::getSymbol(const String& name)
{
    if (!isLoaded())
        return 0;

    return impl_->findSymbol(name);
}

cv::String cv::SharedLibrary::suffix()
{
    return Impl::suffix();
}
