#!/bin/sh

# create build directory
if [ ! -d build ]; then
    mkdir build
fi

# generate hal_impl_templ.h, hal.hpp and hal.cpp files
python gen_hal_wrappers.py

# build static libhal_impl.a
g++ -c hal_impl.cpp
ar rvs build/libhal_impl.a hal_impl.o
rm hal_impl.o

# build hal_user_static app
g++ -Wall -o build/hal_user_static -DCV_HAL_STATIC hal_user.cpp hal.cpp build/libhal_impl.a
