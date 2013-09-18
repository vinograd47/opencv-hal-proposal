include_directories("${CMAKE_SOURCE_DIR}/opencv_module")
include_directories(${OpenCV_INCLUDE_DIRS})

set(all_build_files
    "opencv_user.cpp"
)

add_executable(opencv_user ${all_build_files})

target_link_libraries(opencv_user opencv_module ${OpenCV_LIBS})

set_target_properties(opencv_user PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${bin_output_dir}
)
