cmake_minimum_required(VERSION 3.21)

project(
  qkeg
  VERSION 0.1.0
  LANGUAGES CXX C
)

if(NOT DEFINED Vulkan_ROOT AND DEFINED ENV{VULKAN_SDK} AND EXISTS "$ENV{VULKAN_SDK}")
  set(Vulkan_ROOT "$ENV{VULKAN_SDK}")
elseif(NOT DEFINED Vulkan_ROOT)
  set(qkeg_vulkan_sdk_candidates
    "H:/VulkanSDK"
    "C:/VulkanSDK"
  )

  file(GLOB qkeg_vulkan_sdk_h_children LIST_DIRECTORIES true "H:/VulkanSDK/*")
  file(GLOB qkeg_vulkan_sdk_c_children LIST_DIRECTORIES true "C:/VulkanSDK/*")
  list(APPEND qkeg_vulkan_sdk_candidates
    ${qkeg_vulkan_sdk_h_children}
    ${qkeg_vulkan_sdk_c_children}
  )

  foreach(qkeg_vulkan_sdk_candidate IN LISTS qkeg_vulkan_sdk_candidates)
    if(EXISTS "${qkeg_vulkan_sdk_candidate}/Include/vulkan/vulkan.h")
      set(Vulkan_ROOT "${qkeg_vulkan_sdk_candidate}")
      break()
    endif()
  endforeach()
endif()

if(DEFINED Vulkan_ROOT AND EXISTS "${Vulkan_ROOT}")
  set(ENV{VULKAN_SDK} "${Vulkan_ROOT}")
  list(PREPEND CMAKE_PREFIX_PATH "${Vulkan_ROOT}")
  message(STATUS "Using Vulkan SDK from ${Vulkan_ROOT}")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(CPM_DOWNLOAD_VERSION "0.40.7")
set(CPM_DOWNLOAD_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/cmake/_deps/CPM-${CPM_DOWNLOAD_VERSION}.cmake")
get_filename_component(CPM_DOWNLOAD_DIRECTORY "${CPM_DOWNLOAD_LOCATION}" DIRECTORY)
file(MAKE_DIRECTORY "${CPM_DOWNLOAD_DIRECTORY}")

if(NOT EXISTS "${CPM_DOWNLOAD_LOCATION}")
  message(STATUS "Downloading CPM.cmake ${CPM_DOWNLOAD_VERSION} to ${CPM_DOWNLOAD_LOCATION}")
  file(
    DOWNLOAD
    "https://github.com/cpm-cmake/CPM.cmake/releases/download/v${CPM_DOWNLOAD_VERSION}/CPM.cmake"
    "${CPM_DOWNLOAD_LOCATION}"
    TLS_VERIFY ON
    STATUS CPM_DOWNLOAD_STATUS
  )

  list(GET CPM_DOWNLOAD_STATUS 0 CPM_DOWNLOAD_RESULT)
  list(GET CPM_DOWNLOAD_STATUS 1 CPM_DOWNLOAD_MESSAGE)
  if(NOT CPM_DOWNLOAD_RESULT EQUAL 0)
    file(REMOVE "${CPM_DOWNLOAD_LOCATION}")
    message(FATAL_ERROR "Failed to download CPM.cmake: ${CPM_DOWNLOAD_MESSAGE}")
  endif()
endif()

include("${CPM_DOWNLOAD_LOCATION}")
include(ColorDiagnostics)
include(TargetAddExtraWarnings)
include(TargetShaders)
include(CopyDepsToRuntimeDir)
include(ExeTargetCommon)
include(AssetManagement)
include(EDBRShaders)

if(WIN32)
  set(BUILD_SHARED_LIBS OFF CACHE BOOL "Link to libs dynamically")
else()
  set(BUILD_SHARED_LIBS ON CACHE BOOL "Link to libs dynamically")
endif()

if(MSVC)
  # Disable Just My Code debugging
  add_compile_options(/JMC-)
endif()

option(EDBR_BUILD_TESTING "Build tests" OFF)

add_subdirectory(qkeg)

if(PROJECT_IS_TOP_LEVEL)
  add_subdirectory(games)
endif()
