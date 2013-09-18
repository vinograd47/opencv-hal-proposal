Proof-of-concept design for the OpenCV Hardware Acceleration Layer (HAL) interface.
===================================================================================



To test (Linux only)
--------------------

1. Run `build_static_hal.sh`, `build_shared_hal.sh` and `build_no_hal.sh` scripts.
2. Run `build/hal_user_static`. It will use statically-linked HAL.
3. Run `build/hal_user_shared`. It won't use HAL.
4. Run `build/hal_user_shared ./build/libhal_impl.so`. It will load HAL at runtime.
5. Run `build/hal_user_no`. It won't use any HAL.



Some notes about the HAL design
-------------------------------

* OpenCV can be built in three modes:
  * Without HAL support.
  * With statically-linked HAL.
  * With dynamically-loadable HAL support.

* If OpenCV was built with statically-linked HAL, it will use it.

* If OpenCV was built with dynamically-loadable HAL support,
  the library will try to load HAL at runtime.

* If OpenCV was built without HAL support, it won't use any HAL and
  won't try to load HAL at runtime.

* OpenCV provides HAL interface description (as a set of C header files).
* HAL interface has plain C API. This API is fixed.
  It can be only extended with new functions.
* A HAL implementation doesn't have to implement all operations.
  A HAL developer can implement only subset of this interface.
* Some small functions from HAL are available only in static mode as inlined (cvRound, for example).
* OpenCV can provide separate test suite set for HAL implementations.

* In dynamically-loadable mode the initialization can be explicit or implicit.
  OpenCV user can load own HAL from code (cv::loadHalImpl("myhal.so")).
  Otherwize, HAL will be initilized on the first call.



How it works?
-------------

OpenCV has a special header `hal_interface.h`, which defines HAL interface.
This header is used both in OpenCV and in HAL implementations.
This header can be extended in new OpenCV versions, we can add new functions,
but we can't change existed prototypes.

HAL developer can implement a subset of functions defined in this header.

HAL developer provides own header for the HAL - `hal_impl.h`. This header is used for static build.
Thу header defines macros, describing which functions are provided by the HAL (like `CV_HAL_HAS_RESIZE`).
Also this header can contain some inline funcions (like cvRound).
OpenCV provides a template for this header - `hal_impl_templ.h`

OpenCV has CMake variable `OPENCV_STATIC_HAL`.
If this variable points to some static HAL OpenCV will use this HAL,
otherwize OpenCV will use dynamically-loadable mode,
if `OPENCV_DYNAMIC_HAL` is `TRUE`.

OpenCV has C++ wrappers for HAL API (`hal.hpp` and `hal.cpp`),
that converts OpenCV structures (Mat, vectors, etc.) to HAL types.
All functions return boolean value, which indicates if HAL call was successfull or not.
If HAL fails OpenCV uses own implementation.
This wrappers are generated automatically from `hal_interface.h` by scripts.
