#!/bin/sh

# create build directory
if [ ! -d build ]; then
    mkdir build
fi

# generate hal_impl_templ.h, hal.hpp and hal.cpp files
python gen_hal_wrappers.py

# build shared libhal_impl.so
g++ -Wall -shared -fPIC -o build/libhal_impl.so hal_impl.cpp

# build hal_user_shared app
g++ -Wall -o build/hal_user_shared -DCV_HAL_DYNAMIC hal_user.cpp hal.cpp -ldl -pthread
