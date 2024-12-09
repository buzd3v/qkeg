find_package(Vulkan REQUIRED)
add_subdirectory(third_party)

# add_subdirectory(tools)
add_library(qkeg

  # main files
  src/Application.cpp
  src/Graphics/Buffers.cpp

  # JsonParser files
  src/JsonParser/JsonNode.cpp
  src/JsonParser/JsonFile.cpp

  # Input files
  src/Input/ActionMapping.cpp
  src/Input/ButtonState.cpp
  src/Input/KeyboardState.cpp
  src/Input/InputManager.cpp

  # graphics files
  src/Graphics/GPUDevice.cpp
  src/Graphics/Vulkan/Swapchain.cpp
  src/Graphics/Vulkan/GPUImage.cpp
  src/Graphics/Vulkan/VkInitializer.cpp
  src/Graphics/Vulkan/ImagePool.cpp
  src/Graphics/Vulkan/VkUtil.cpp
  src/Graphics/Vulkan/VkDebugUtil.cpp
  src/Graphics/Vulkan/PipelineBuilder.cpp
  src/Graphics/Vulkan/ImageStream.cpp
  src/Graphics/Vulkan/BindlessDescriptor.cpp
  src/Graphics/Vulkan/GPUBuffer.cpp
  src/Graphics/Vulkan/VkExcutor.cpp
  src/Graphics/GLTFLoader/GLTFLoader.cpp

  # mesh file
  src/Graphics/Mesh/MeshPool.cpp
  src/Graphics/Mesh/MaterialPool.cpp

  # pipeline files
  src/Graphics/Pipeline/GradientPipeline.cpp
  src/Graphics/Pipeline/MeshPipeline.cpp

  # renderer files
  src/Graphics/Renderer/GameRenderer.cpp
  src/Graphics/Renderer/Scene.cpp
  src/Graphics/Renderer/Light.cpp
  src/Graphics/Renderer/ScenePool.cpp
  src/Graphics/Renderer/Skeleton.cpp
  src/Graphics/Camera/Camera.cpp

  # math files
  src/Math/Transform.cpp
)

add_library(qkeg::qkeg ALIAS qkeg)

set_target_properties(qkeg PROPERTIES
  CXX_STANDARD 20
  CXX_EXTENSIONS OFF
)

target_add_extra_warnings(qkeg)

target_include_directories(qkeg
  PUBLIC
  "${CMAKE_CURRENT_LIST_DIR}/include"
  "${CMAKE_CURRENT_LIST_DIR}/include/Graphics"
  "${CMAKE_CURRENT_LIST_DIR}/third_party"
  "${CMAKE_CURRENT_LIST_DIR}/third_party/singleton/include"

  # "${CMAKE_CURRENT_LIST_DIR}/include"
  PRIVATE
  ${Vulkan_INCLUDE_DIRS}
)

target_link_libraries(qkeg
  PUBLIC
  glm::glm
  glfw
  fmt::fmt
  nlohmann_json::nlohmann_json
  volk::volk_headers
  vk-bootstrap::vk-bootstrap
  GPUOpen::VulkanMemoryAllocator
  PRIVATE
  Vulkan::Vulkan
  singleton::singleton-atomic
  fastgltf::fastgltf
)

target_compile_definitions(qkeg PUBLIC VK_NO_PROTOTYPES VK_USE_PLATFORM_WIN32_KHR VMA_VULKAN_VERSION=1003000 NOMINMAX)

# target_compile_definitions(qkeg PUBLIC AL_ALEXT_PROTOTYPES)

# target_compile_definitions(qkeg
# PUBLIC
# VK_NO_PROTOTYPES
# VMA_VULKAN_VERSION=1003000

# # VOLK_DEFAULT_VISIBILITY # FIXME: doesn't work for some reason
# )

# # SDL
# if(BUILD_SHARED_LIBS)
# target_link_libraries(qkeg PUBLIC
# SDL2::SDL2
# )
# else()
# target_link_libraries(qkeg PUBLIC
# SDL2::SDL2-static
# )
# endif()

# if(WIN32)
# target_link_libraries(qkeg PRIVATE SDL2::SDL2main)
# endif()

# glm
target_compile_definitions(qkeg
  PUBLIC
  GLM_FORCE_CTOR_INIT
  GLM_FORCE_XYZW_ONLY
  GLM_FORCE_EXPLICIT_CTOR
  GLM_FORCE_DEPTH_ZERO_TO_ONE
  GLM_ENABLE_EXPERIMENTAL
)

# target_compile_definitions(qkeg
# PUBLIC
# JPH_DEBUG_RENDERER
# )

# target_link_libraries(qkeg PUBLIC Tracy::TracyClient)

# if(MSVC)
# target_compile_definitions(TracyClient PUBLIC
# $<$<CONFIG:Debug,RelWithDebInfo>:TRACY_ENABLE>
# $<$<CONFIG:Debug,RelWithDebInfo>:TRACY_DELAYED_INIT> # for unit tests to work
# )
# else()
# if(NOT CMAKE_BUILD_TYPE OR
# CMAKE_BUILD_TYPE STREQUAL "Debug" OR
# CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
# target_compile_definitions(TracyClient PUBLIC
# TRACY_ENABLE
# TRACY_DELAYED_INIT # needed for unit tests to work
# )
# endif()
# endif()

# # # testing
# include(CTest)

# if(EDBR_BUILD_TESTING)
# enable_testing()

# if(NOT MSVC)
# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure)
# else()
# add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} -C Debug --output-on-failure)
# endif()

# add_subdirectory(test)
# endif()
