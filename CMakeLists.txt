cmake_minimum_required(VERSION 2.8.10)

project(opencv-hal-proposal)

set(bin_output_dir "${CMAKE_BINARY_DIR}/bin")
set(lib_output_dir "${CMAKE_BINARY_DIR}/lib")

#find_package(OpenCV REQUIRED)
find_package(Threads REQUIRED)
find_package(PythonInterp)

if(PYTHONINTERP_FOUND)
    option(GENERATE_HAL_WRAPPERS "Generate HAL wrappers" TRUE)
endif()

set(hal_modes "No HAL" "Static HAL" "Dynamic HAL")
set(HAL_MODE "No HAL" CACHE STRING "HAL mode")
set_property(CACHE HAL_MODE PROPERTY STRINGS ${hal_modes})

if(HAL_MODE STREQUAL "Static HAL")
    set(static_hal TRUE)
elseif(HAL_MODE STREQUAL "Dynamic HAL")
    set(dynamic_hal TRUE)
endif()

add_subdirectory(hal_api)
add_subdirectory(hal_impl)
add_subdirectory(opencv_module)
add_subdirectory(opencv_user)
