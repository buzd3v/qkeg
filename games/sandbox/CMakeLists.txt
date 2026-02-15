include(CMakePrintHelpers)
cmake_minimum_required(VERSION 3.21)

project(
  sandbox
  VERSION 0.1.0
  LANGUAGES CXX C
)

if(PROJECT_IS_TOP_LEVEL)
  set(qkeg_dir "${CMAKE_CURRENT_LIST_DIR}/../..")
  add_subdirectory("${qkeg_dir}" "${CMAKE_CURRENT_BINARY_DIR}/qkeg")
endif()

add_executable(sandbox

  # gamefile
  src/Game.cpp
  src/main.cpp
  src/EntityInit.cpp
  src/InputHandler.cpp
  src/CameraManager.cpp
  src/DynamicCameraHandler.cpp
  src/TrackingCameraHandler.cpp
  src/GameUtil.cpp

  src/Levels/EntryLevel.cpp
  src/Levels/MainLevel.cpp
)

set_target_properties(sandbox PROPERTIES
  CXX_STANDARD 20
  CXX_EXTENSIONS OFF
)

target_add_extra_warnings(sandbox)

target_include_directories(sandbox PRIVATE "${CMAKE_CURRENT_LIST_DIR}/src")

target_link_libraries(sandbox PRIVATE
  qkeg::qkeg
)

symlink_assets(sandbox)

# build shaders
get_qkeg_common_3d_shaders(QKEG_3D_SHADERS)
message(STATUS "${QKEG_3D_SHADERS}")
target_shaders(sandbox "${QKEG_3D_SHADERS}")

# set_exe_target_common_props(mtpgame)

# include("${CMAKE_CURRENT_LIST_DIR}/MTPInstall.cmake")
include(CTest)

if(EDBR_BUILD_TESTING)
  enable_testing()
  add_subdirectory(test)
endif()
