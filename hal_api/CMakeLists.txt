if(GENERATE_HAL_WRAPPERS)
    add_custom_target(hal_api ALL
                      ${PYTHON_EXECUTABLE} "${CMAKE_SOURCE_DIR}/scripts/gen_hal_wrappers.py"
                                           "${CMAKE_SOURCE_DIR}/hal_api/hal_interface.h"
                                           "${CMAKE_SOURCE_DIR}/hal_api/hal_impl_templ.h"
                                           "${CMAKE_SOURCE_DIR}/opencv_module/hal.hpp"
                                           "${CMAKE_SOURCE_DIR}/opencv_module/hal.cpp"
                      DEPENDS "${CMAKE_SOURCE_DIR}/scripts/gen_hal_wrappers.py" "${CMAKE_SOURCE_DIR}/hal_api/hal_interface.h"
                      COMMENT "Generate HAL wrappers"
                      VERBATIM
                      SOURCES "${CMAKE_SOURCE_DIR}/scripts/gen_hal_wrappers.py" "${CMAKE_SOURCE_DIR}/hal_api/hal_interface.h")
endif()
