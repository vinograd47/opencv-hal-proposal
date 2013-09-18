#ifndef CORE_HPP
#define CORE_HPP

#include <cstdlib>
#include <string>

namespace cv
{
    // This function is not thread safe.
    void loadHalImpl(const std::string& halLibName);

    std::string getHalInfo();

///////////////////////////////////
    class Mat
    {
    public:
        Mat()
        {
            data = NULL;
            step = 0;
            rows = 0;
            cols = 0;
            depth = 0;
            channels = 1;
        }

        unsigned char* data;
        size_t step;
        int rows;
        int cols;
        int depth;
        int channels;
    };

    class Size
    {
    public:
        int width;
        int height;

        Size()
        {
            width = 0;
            height = 0;
        }
    };

    class Point
    {
    public:
        int x;
        int y;

        Point()
        {
            x = 0;
            y = 0;
        }
    };

    static inline int cvRound(double val)
    {
        return (int) (val + 0.5);
    }
}

#endif
