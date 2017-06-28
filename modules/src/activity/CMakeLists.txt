project(vtmodule_activity)
cmake_minimum_required(VERSION 2.8)

file(GLOB_RECURSE ${PROJECT_NAME}.HeadersFramework SummarizationFramework/*.h )

# detection module
add_library(${PROJECT_NAME}_det SHARED
    ActivityDetection.cpp
    ActivityDetection.h
    VtVideoAdapter.cpp
    VtVideoAdapter.h
    VtOutputHandler.cpp
    VtOutputHandler.h
    ActivityDefs.h
    ${CommonSources}
    ${DetectionSources}
    ${${PROJECT_NAME}.HeadersFramework}
)

if (OPENCV_LIBDIR)
  set_target_properties(${PROJECT_NAME}_det PROPERTIES
    INSTALL_RPATH ${OPENCV_LIBDIR}
)
endif()

target_link_libraries(${PROJECT_NAME}_det
    vtapi
    ${OPENCV_LIBS}
)

target_include_directories(${PROJECT_NAME}_det BEFORE PUBLIC
  ${OPENCV_INCLUDE_PATH}
)

#install rules
install(TARGETS ${PROJECT_NAME}_det
    DESTINATION ${VTAPI_MODULES_DIR})

