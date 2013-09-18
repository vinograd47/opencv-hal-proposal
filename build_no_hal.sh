#!/bin/sh

# create build directory
if [ ! -d build ]; then
    mkdir build
fi

# generate hal_impl_templ.h, hal.hpp and hal.cpp files
python gen_hal_wrappers.py

# build hal_user_no app
g++ -Wall -o build/hal_user_no hal_user.cpp hal.cpp
