#ifndef __OPENCV_UTILITY_HPP__
#define __OPENCV_UTILITY_HPP__

#include <opencv2/core.hpp>

namespace cv {

class CV_EXPORTS SharedLibrary
{
public:
    SharedLibrary();
    SharedLibrary(const String& path);

    bool load(const String& path);
    void unload();

    bool isLoaded() const;

    void* getSymbol(const String& name);

    static String suffix();

private:
    class Impl;
    Ptr<Impl> impl_;
};

}

#endif
