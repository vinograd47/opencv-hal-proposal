// This header belongs to HAL implementation.
// This header is used for static build.

// It includes hal_interface.h header and defines macros,
// describing which functions are provided by this HAL.

// Also it can contain some inline functions,
// that will be used in static build.

#ifndef HAL_IMPL_H
#define HAL_IMPL_H

#include <iostream>
#include "hal_interface.h"

// Provided functions

//#define CV_HAL_HAS_HAMMING_DIST
#define CV_HAL_HAS_RESIZE
#define CV_HAL_HAS_ERODE

// Inline functions

#define CV_HAL_HAS_ROUND
CV_HAL_INLINE int cvhal_round(double val)
{
    std::cout << "round from HAL" << std::endl;
    return (int) (val + 0.5);
}

#endif
