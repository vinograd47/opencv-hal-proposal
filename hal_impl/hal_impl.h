// This header belongs to HAL implementation.

// It includes hal_interface.h header and defines macros,
// describing which functions are provided by this HAL.

// This header is used for static build.

#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include <iostream>
#include "hal_interface.h"

//#define CV_HAL_HAS_HAMMING_DIST
#define CV_HAL_HAS_RESIZE
#define CV_HAL_HAS_ERODE

// This functions can be used only in static build as inlined

#define CV_HAL_HAS_ROUND
CV_HAL_INLINE int cvhal_round(double val)
{
    std::cout << "round from HAL" << std::endl;
    return (int) (val + 0.5);
}

#endif
