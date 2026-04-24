if(NOT COMMAND CPMAddPackage)
  include("${CMAKE_CURRENT_LIST_DIR}/CPM.cmake")
endif()

# glm
CPMAddPackage(
  NAME glm
  GITHUB_REPOSITORY g-truc/glm
  GIT_TAG 33b4a621a697a305bc3a7610d290677b96beb181
  OPTIONS
    "GLM_BUILD_TESTS OFF"
)

# glfw
CPMAddPackage(
  NAME glfw
  GITHUB_REPOSITORY glfw/glfw
  GIT_TAG 3.4
  OPTIONS
    "GLFW_BUILD_DOCS OFF"
    "GLFW_BUILD_TESTS OFF"
    "GLFW_BUILD_EXAMPLES OFF"
    "GLFW_INSTALL OFF"
)

# stb
# The recorded submodule pin 2e2bef463a5b53ddf8bb788e25da6b8506314c08 is no
# longer fetchable upstream, so use the current working remote HEAD instead.
CPMAddPackage(
  NAME stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG master
  DOWNLOAD_ONLY YES
)

# if(NOT TARGET stb::image)
#   add_library(stb_image INTERFACE)
#   add_library(stb::image ALIAS stb_image)
#   target_include_directories(stb_image INTERFACE "${stb_SOURCE_DIR}")
# endif()

# singleton
CPMAddPackage(
  NAME singleton
  GITHUB_REPOSITORY jimmy-park/singleton
  GIT_TAG 9d5420e558ec0d966652f7ec55fbb344038d8046
)

# entt
CPMAddPackage(
  NAME entt
  GITHUB_REPOSITORY skypjack/entt
  GIT_TAG 42d9628d39c579ae9f337018a96dc74b7122a496
)

# # SDL
# if (NOT BUILD_SHARED_LIBS)
# set(SDL_SHARED_ENABLED_BY_DEFAULT OFF CACHE BOOL "Don't build SDL as shared lib")
# endif()
# option(SDL_TEST "Build the SDL2_test library" OFF)
# option(SDL_AUDIO_ENABLED_BY_DEFAULT "Enable the Audio subsystem" OFF)
# add_subdirectory(SDL)

# vk-bootstrap
CPMAddPackage(
  NAME vk-bootstrap
  GITHUB_REPOSITORY charles-lunarg/vk-bootstrap
  GIT_TAG e3d07c49cbfb93ea62d2ec0a52c6d908c00d5269
  OPTIONS
    "VK_BOOTSTRAP_TEST OFF"
    "VK_BOOTSTRAP_INSTALL OFF"
)

if(MSVC)
    target_compile_definitions(vk-bootstrap PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
endif()

if(BUILD_SHARED_LIBS)
    set_target_properties(vk-bootstrap PROPERTIES
        POSITION_INDEPENDENT_CODE ON
    )
endif()

# vma
CPMAddPackage(
  NAME VMA
  GITHUB_REPOSITORY GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
  GIT_TAG 1c35ba99ce775f8342d87a83a3f0f696f99c2a39
  OPTIONS
    "VMA_ENABLE_INSTALL OFF"
)

# volk
CPMAddPackage(
  NAME volk
  GITHUB_REPOSITORY zeux/volk
  GIT_TAG 086957f3b39ac0a37fb7f344a523bef9b194ba8f
  OPTIONS
    "VOLK_INSTALL OFF"
)

# fastgltf
# The recorded submodule pin 9d56da8490bf9534009e9425851c82e8c0d217a8 is no
# longer fetchable upstream, so use the current working remote commit instead.
CPMAddPackage(
  NAME fastgltf
  GITHUB_REPOSITORY spnda/fastgltf
  GIT_TAG 48b3ac7c86d552c95186b366d107fa2dc7ef1249
  OPTIONS
    "FASTGLTF_ENABLE_TESTS OFF"
    "FASTGLTF_ENABLE_EXAMPLES OFF"
    "FASTGLTF_ENABLE_DOCS OFF"
)

if(MSVC)
    target_compile_definitions(fastgltf PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
endif()

# raudio
# raylib 5.5 no longer ships raudio.h; fetch the standalone audio module and
# wrap it as a normal CMake target.
# CPMAddPackage(
#   NAME raudio
#   GITHUB_REPOSITORY raysan5/raudio
#   GIT_TAG 811c150fe97a2abb8ec67857dc52e022bfaa4e43
#   DOWNLOAD_ONLY YES
# )

# if(NOT TARGET raudio)
#   add_library(raudio STATIC "${raudio_SOURCE_DIR}/src/raudio.c")
#   add_library(raudio::raudio ALIAS raudio)
#   target_include_directories(raudio PUBLIC "${raudio_SOURCE_DIR}/src")
#   target_compile_definitions(raudio
#     PRIVATE
#       SUPPORT_MODULE_RAUDIO
#       RAUDIO_STANDALONE
#       SUPPORT_FILEFORMAT_WAV
#       SUPPORT_FILEFORMAT_OGG
#       SUPPORT_FILEFORMAT_MP3
#       SUPPORT_FILEFORMAT_QOA
#       SUPPORT_FILEFORMAT_FLAC
#       SUPPORT_FILEFORMAT_XM
#       SUPPORT_FILEFORMAT_MOD
#   )
# endif()

# ## Dear ImGui
# set(IMGUI_SOURCES
# "imgui.cpp"
# "imgui_draw.cpp"
# "imgui_tables.cpp"
# "imgui_widgets.cpp"
# "imgui_demo.cpp"
# # C++ additions
# "misc/cpp/imgui_stdlib.cpp"
# # backend
# "backends/imgui_impl_sdl2.cpp"
# )

# list(TRANSFORM IMGUI_SOURCES PREPEND "${CMAKE_CURRENT_SOURCE_DIR}/imgui/")

# add_library(imgui STATIC ${IMGUI_SOURCES})
# set_target_properties(imgui PROPERTIES POSITION_INDEPENDENT_CODE ON)

# target_include_directories(imgui
# PUBLIC
# "imgui"
# "imgui/backends"
# )

# if(BUILD_SHARED_LIBS)
# target_link_libraries(imgui
# PUBLIC
# SDL2::SDL2
# )
# else()
# target_link_libraries(imgui
# PUBLIC
# SDL2::SDL2-static
# )
# endif()

# add_library(imgui::imgui ALIAS imgui)

# ## im3d
# add_library(im3d
# "${CMAKE_CURRENT_LIST_DIR}/im3d/im3d.cpp"
# )
# add_library(im3d::im3d ALIAS im3d)
# if (MSVC)
# target_compile_definitions(im3d PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# endif()

# target_include_directories(im3d PUBLIC
# "${CMAKE_CURRENT_LIST_DIR}/im3d"
# )

# ## tinygltf
# option(TINYGLTF_BUILD_LOADER_EXAMPLE "Build loader_example(load glTF and dump infos)" OFF)
# option(TINYGLTF_INSTALL "Install tinygltf files during install step." OFF)
# option(TINYGLTF_HEADER_ONLY "On: header-only mode. Off: create tinygltf library(No TINYGLTF_IMPLEMENTATION required in your project)" ON)
# add_subdirectory(tinygltf)
# add_library(tinygltf::tinygltf ALIAS tinygltf)

# ## tracy
# option(TRACY_ENABLE "" OFF)
# option(TRACY_ON_DEMAND "" OFF)
# add_subdirectory(tracy)
# if (MSVC)
# target_compile_definitions(TracyClient PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# endif()

# ## freetype
# option(FT_DISABLE_ZLIB
# "Disable use of system zlib and use internal zlib library instead." ON)
# option(FT_DISABLE_BZIP2
# "Disable support of bzip2 compressed fonts." ON)
# option(FT_DISABLE_BROTLI
# "Disable support of compressed WOFF2 fonts." ON)
# option(FT_DISABLE_HARFBUZZ
# "Disable HarfBuzz (used for improving auto-hinting of OpenType fonts)." ON)
# option(FT_DISABLE_PNG
# "Disable support of PNG compressed OpenType embedded bitmaps." ON)
# option(SKIP_INSTALL_ALL "Skip install all" ON)
# option(FT_ENABLE_ERROR_STRINGS
# "Enable support for meaningful error descriptions." ON)
# add_subdirectory(freetype)
# add_library(freetype::freetype ALIAS freetype)

# ## utfcpp
# add_subdirectory(utfcpp)
# add_library(utf8cpp::utf8cpp ALIAS utf8cpp)

# ## entt
# add_subdirectory(entt)

# json
CPMAddPackage(
  NAME nlohmann_json
  GITHUB_REPOSITORY nlohmann/json
  GIT_TAG b36f4c477c40356a0ae1204b567cca3c2a57d201
  OPTIONS
    "JSON_MultipleHeaders ON"
    "JSON_Install OFF"
    "JSON_BuildTests OFF"
)

# fmt
CPMAddPackage(
  NAME fmt
  GITHUB_REPOSITORY fmtlib/fmt
  GIT_TAG c90bc918620b9b15d922f51f5f46667f9f588668
  OPTIONS
    "FMT_DOC OFF"
    "FMT_TEST OFF"
    "FMT_INSTALL OFF"
)
set_target_properties(fmt PROPERTIES
    PUBLIC_HEADER ""
)
target_compile_options(fmt PRIVATE
    $<$<CXX_COMPILER_ID:GNU>:-w>
    $<$<CXX_COMPILER_ID:Clang>:-w>
    $<$<CXX_COMPILER_ID:MSVC>:/w>
)

if(MSVC)
    target_compile_definitions(fmt PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
endif()

# ## jolt
# if(NOT CMAKE_BUILD_TYPE OR
# CMAKE_BUILD_TYPE STREQUAL "Debug" OR
# CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
# option(GENERATE_DEBUG_SYMBOLS "Generate debug symbols" ON)
# else()
# option(GENERATE_DEBUG_SYMBOLS "Generate debug symbols" OFF)
# endif()
# option(OVERRIDE_CXX_FLAGS "Override CMAKE_CXX_FLAGS_DEBUG/RELEASE" OFF)
# option(ENABLE_ALL_WARNINGS "Enable all warnings and warnings as errors" OFF)
# option(USE_STATIC_MSVC_RUNTIME_LIBRARY "Use the static MSVC runtime library" OFF)
# option(DEBUG_RENDERER_IN_DISTRIBUTION "Enable debug renderer in Distribution builds" ON)
# add_subdirectory(JoltPhysics/Build)
# add_library(Jolt::Jolt ALIAS Jolt)
# if (MSVC)
# target_compile_definitions(Jolt PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# endif()

# ## openal-soft

# # Disable PipeWire because if you build on systems where pipewire is installed,
# # then it won't work on systems without PipeWire
# option(ALSOFT_BACKEND_PIPEWIRE "Enable PipeWire backend" OFF)

# # disabled SNDIO, most Linuxes should have ALSA or PulseAudio
# option(ALSOFT_BACKEND_SNDIO "Enable SndIO backend" OFF)

# option(ALSOFT_UTILS "Build utility programs"  OFF)
# option(ALSOFT_EXAMPLES  "Build example programs"  OFF)
# option(ALSOFT_INSTALL "Install main library" OFF)
# option(ALSOFT_INSTALL_CONFIG "Install alsoft.conf sample configuration file" OFF)
# option(ALSOFT_INSTALL_HRTF_DATA "Install HRTF data files" OFF)
# option(ALSOFT_INSTALL_AMBDEC_PRESETS "Install AmbDec preset files" OFF)
# option(ALSOFT_INSTALL_EXAMPLES "Install example programs (alplay, alstream, ...)" OFF)
# option(ALSOFT_INSTALL_UTILS "Install utility programs (openal-info, alsoft-config, ...)" OFF)
# option(ALSOFT_UPDATE_BUILD_VERSION "Update git build version info" OFF)

# add_subdirectory(openal-soft)
# if (MSVC)
# target_compile_definitions(alcommon PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# target_compile_definitions(OpenAL PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# endif()

# # mute some annoying warnings
# include(OpenALSoftFlags)
# open_al_soft_flags(alcommon)
# open_al_soft_flags(OpenAL)

# ## ogg
# option(INSTALL_DOCS "Install documentation" OFF)
# option(INSTALL_PKG_CONFIG_MODULE "Install ogg.pc file" OFF)
# option(INSTALL_CMAKE_PACKAGE_MODULE "Install CMake package configuration module" OFF)
# add_subdirectory(ogg)

# set_target_properties(ogg PROPERTIES
# PUBLIC_HEADER ""
# )

# ## libvorbis
# option(INSTALL_CMAKE_PACKAGE_MODULE "Install CMake package configiguration module" OFF)
# add_subdirectory(vorbis)
# add_library(vorbis::vorbis ALIAS vorbis)
# add_library(vorbis::vorbisfile ALIAS vorbisfile)

# ## Google test
# if(EDBR_BUILD_TESTING)
# if (MSVC AND NOT USE_STATIC_RUNTIME)
# set(gtest_force_shared_crt ON CACHE BOOL "Use shared CRT")
# endif()

# add_subdirectory(googletest)
# if (MSVC)
# target_compile_definitions(gtest PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# target_compile_definitions(gtest_main PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# target_compile_definitions(gmock PRIVATE $<$<CONFIG:Debug>:_ITERATOR_DEBUG_LEVEL=1>)
# endif()
# endif()

# ## CLI11
# add_subdirectory(CLI11)

# # cute_headers (Aseprite support)
# add_subdirectory(cute_headers)
