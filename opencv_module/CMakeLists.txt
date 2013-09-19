include_directories("${CMAKE_SOURCE_DIR}/hal_api")
include_directories(${OpenCV_INCLUDE_DIRS})

set(all_build_files
    "opencv_module.hpp"
    "opencv_module.cpp"
    "shared_library.hpp"
    "shared_library.cpp"
    "hal.hpp"
    "hal.cpp"
)

add_definitions("-D__OPENCV_BUILD")

if(static_hal)
    include_directories(${STATIC_HAL_INCLUDE_DIR})
    add_definitions("-DCV_HAL_STATIC")
    add_library(opencv_module STATIC ${all_build_files})
    target_link_libraries(opencv_module ${STATIC_HAL_LIB})
elseif(dynamic_hal)
    add_definitions("-DCV_HAL_DYNAMIC" "-DCVAPI_EXPORTS")
    add_library(opencv_module SHARED ${all_build_files})
else()
    add_library(opencv_module STATIC ${all_build_files})
endif()

if(GENERATE_HAL_WRAPPERS)
    add_dependencies(opencv_module hal_api)
endif()

target_link_libraries(opencv_module ${CMAKE_THREAD_LIBS_INIT} ${CMAKE_DL_LIBS} ${OpenCV_LIBS})

set_target_properties(opencv_module PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${bin_output_dir}
    LIBRARY_OUTPUT_DIRECTORY ${lib_output_dir}
    ARCHIVE_OUTPUT_DIRECTORY ${lib_output_dir}
)
