# common sources for modules
set(CommonSources
  ../ProgressUpdater.cpp
  ../ProgressUpdater.h
  ../interfaces/IProgressUpdater.h)
set(DetectionSources
  ../DetectionModule.cpp
  ../DetectionModule.h
  ../interfaces/IDetectionModule.h)

# common include directories
include_directories(
  interfaces
  ${VTAPI_INCLUDE_PATH}
)

add_subdirectory(activity)
