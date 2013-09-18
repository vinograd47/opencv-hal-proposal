include_directories("${CMAKE_SOURCE_DIR}/hal_api")

set(all_build_files
    "hal_impl.h"
    "hal_impl.cpp"
)

if(static_hal)
    add_library(hal_impl STATIC ${all_build_files})

    set_target_properties(hal_impl PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY ${lib_output_dir}
    )
elseif(dynamic_hal)
    add_library(hal_impl SHARED ${all_build_files})

    set_target_properties(hal_impl PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${bin_output_dir}
        LIBRARY_OUTPUT_DIRECTORY ${lib_output_dir}
        ARCHIVE_OUTPUT_DIRECTORY ${lib_output_dir}
    )
endif()
