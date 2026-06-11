if(EMSCRIPTEN)
  message("Using Emscripten!")
endif()

file(GLOB ENGINE_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

add_library(engine STATIC ${ENGINE_SOURCES}
        EngineHook.cpp
        EngineHook.h)

target_compile_features(engine PUBLIC cxx_std_20)

include(FetchContent)
if(NOT EMSCRIPTEN)
  find_package(Threads REQUIRED)
endif()

# add glm
find_package(glm CONFIG QUIET)
if(NOT glm_FOUND)
  FetchContent_Declare(
    glm
    URL https://github.com/g-truc/glm/releases/download/1.0.3/glm-1.0.3.zip
    DOWNLOAD_NO_PROGRESS ON
    DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
  )
  FetchContent_MakeAvailable(glm)
endif()

# Add SDL3
find_package(SDL3 3.4 CONFIG QUIET)
if(NOT SDL3_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3
      URL https://www.libsdl.org/release/SDL3-devel-3.4.0-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3)
    set(SDL3_DIR "${sdl3_SOURCE_DIR}/cmake")
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_SOURCE_DIR}" "${SDL3_DIR}")
    find_package(SDL3 CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3
      GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
      GIT_TAG release-3.4.0
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    FetchContent_MakeAvailable(SDL3)
  endif()
endif()

if(TARGET SDL3::SDL3)
  set(MINIGIN_SDL3_TARGET SDL3::SDL3)
elseif(TARGET SDL3::SDL3-shared)
  set(MINIGIN_SDL3_TARGET SDL3::SDL3-shared)
else()
  message(FATAL_ERROR "SDL3 target was not found.")
endif()

# Add SDL3_ttf
find_package(SDL3_ttf 3.2.2 CONFIG QUIET)
if(NOT SDL3_ttf_FOUND)
  if (WIN32)
    FetchContent_Declare(
      SDL3_ttf
      URL https://www.libsdl.org/projects/SDL_ttf/release/SDL3_ttf-devel-3.2.2-VC.zip
      DOWNLOAD_NO_PROGRESS ON
      DOWNLOAD_DIR ${CMAKE_BINARY_DIR}/downloads
    )
    FetchContent_MakeAvailable(SDL3_ttf)
    set(SDL3_ttf_DIR "${sdl3_ttf_SOURCE_DIR}/cmake")
    list(PREPEND CMAKE_PREFIX_PATH "${sdl3_ttf_SOURCE_DIR}" "${SDL3_ttf_DIR}")
    find_package(SDL3_ttf CONFIG REQUIRED)
  else()
    FetchContent_Declare(
      SDL3_ttf
      GIT_REPOSITORY https://github.com/libsdl-org/SDL_ttf.git
      GIT_TAG release-3.2.2
      GIT_SHALLOW TRUE
      GIT_PROGRESS TRUE
    )
    set(SDL3TTF_INSTALL OFF)
    if(EMSCRIPTEN)
      list(APPEND CMAKE_MODULE_PATH "../cmake/emscripten")
      set(SDL3TTF_USE_SYSTEM_FREETYPE ON)
      set(SDL3TTF_VENDORED OFF)
      set(SDLTTF_PLUTOSVG OFF)
      add_compile_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
      add_link_options(-sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1)
    endif()
    FetchContent_MakeAvailable(SDL3_ttf)
  endif()
endif()

if(TARGET SDL3_ttf::SDL3_ttf)
  set(MINIGIN_SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf)
elseif(TARGET SDL3_ttf::SDL3_ttf-shared)
  set(MINIGIN_SDL3_TTF_TARGET SDL3_ttf::SDL3_ttf-shared)
else()
  message(FATAL_ERROR "SDL3_ttf target was not found.")
endif()

option(MINIGIN_ENABLE_VLD "Enable Visual Leak Detector when available" OFF)

if (WIN32 AND MINIGIN_ENABLE_VLD)
  # Add VLD (Visual Leak Detector) for memory leak detection on Windows.
  # The normal installer does not ship a VLDConfig.cmake, so find the classic
  # include/lib/bin layout directly instead of requiring VLD_DIR.
  set(VLD_ROOT "" CACHE PATH "Visual Leak Detector install directory")
  if(NOT VLD_ROOT AND EXISTS "C:/Program Files (x86)/Visual Leak Detector")
    set(VLD_ROOT "C:/Program Files (x86)/Visual Leak Detector" CACHE PATH "Visual Leak Detector install directory" FORCE)
  endif()

  if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(MINIGIN_VLD_ARCH "Win64")
  else()
    set(MINIGIN_VLD_ARCH "Win32")
  endif()

  find_path(VLD_INCLUDE_DIR
    NAMES vld.h
    PATHS "${VLD_ROOT}/include"
    NO_DEFAULT_PATH
  )
  find_library(VLD_LIBRARIES
    NAMES vld
    PATHS "${VLD_ROOT}/lib/${MINIGIN_VLD_ARCH}"
    NO_DEFAULT_PATH
  )

  if(VLD_INCLUDE_DIR AND VLD_LIBRARIES)
    set(MINIGIN_VLD_DLL_DIR "${VLD_ROOT}/bin/${MINIGIN_VLD_ARCH}" CACHE PATH "Visual Leak Detector DLL directory")
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
      set(MINIGIN_VLD_DLL_NAME "vld_x64.dll" CACHE STRING "Visual Leak Detector DLL name")
    else()
      set(MINIGIN_VLD_DLL_NAME "vld_x86.dll" CACHE STRING "Visual Leak Detector DLL name")
    endif()
    message(STATUS "Visual Leak Detector found: ${VLD_ROOT}")
  else()
    message(STATUS "Visual Leak Detector not found. Set VLD_ROOT to enable it.")
    unset(VLD_INCLUDE_DIR CACHE)
    unset(VLD_LIBRARIES CACHE)
  endif()
endif()

if(WIN32 AND NOT MINIGIN_ENABLE_VLD)
  message(STATUS "Visual Leak Detector disabled. Set MINIGIN_ENABLE_VLD=ON to enable it.")
endif()

## Add steam if available
#list(APPEND CMAKE_MODULE_PATH "../cmake/steam")

FetchContent_Declare(
        imgui_src
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.92.5-docking
        GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(imgui_src)

add_library(imgui STATIC
        ${imgui_src_SOURCE_DIR}/imgui.cpp
        ${imgui_src_SOURCE_DIR}/imgui_draw.cpp
        ${imgui_src_SOURCE_DIR}/imgui_tables.cpp
        ${imgui_src_SOURCE_DIR}/imgui_widgets.cpp
        ${imgui_src_SOURCE_DIR}/imgui_demo.cpp
        ${imgui_src_SOURCE_DIR}/backends/imgui_impl_sdl3.cpp
        ${imgui_src_SOURCE_DIR}/backends/imgui_impl_sdlrenderer3.cpp
)

target_include_directories(imgui PUBLIC
        ${imgui_src_SOURCE_DIR}
        ${imgui_src_SOURCE_DIR}/backends
)
target_link_libraries(imgui PUBLIC ${MINIGIN_SDL3_TARGET})

FetchContent_Declare(
        implot_src
        GIT_REPOSITORY https://github.com/epezent/implot.git
        GIT_TAG v0.17
)

FetchContent_MakeAvailable(implot_src)

add_library(implot
        ${implot_src_SOURCE_DIR}/implot.cpp
        ${implot_src_SOURCE_DIR}/implot_items.cpp
)

target_include_directories(implot PUBLIC
        ${implot_src_SOURCE_DIR}
        ${imgui_src_SOURCE_DIR}
)

target_link_libraries(implot PUBLIC imgui)

target_include_directories(engine PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${imgui_src_SOURCE_DIR}
        ${implot_src_SOURCE_DIR}
)

target_link_libraries(engine PUBLIC
        ${MINIGIN_SDL3_TARGET}
        ${MINIGIN_SDL3_TTF_TARGET}
        glm::glm
        imgui
        implot
)

if(MINIGIN_ENABLE_VLD AND VLD_INCLUDE_DIR AND VLD_LIBRARIES)
  target_include_directories(engine PUBLIC ${VLD_INCLUDE_DIR})
  target_link_libraries(engine PUBLIC ${VLD_LIBRARIES})
endif()

if(NOT EMSCRIPTEN)
  target_link_libraries(engine PUBLIC Threads::Threads)
endif()

if(MSVC)
  # compile at warning level 4 + treat warnings as errors
  target_compile_options(engine PRIVATE /W4 /WX)
else()
  target_compile_options(engine PRIVATE -Wall -Wextra -Werror)
endif()

if(EMSCRIPTEN)
  target_compile_options(engine PUBLIC -g)
  target_link_options(engine PUBLIC -sUSE_FREETYPE=1 -sUSE_HARFBUZZ=1 -g -sALLOW_MEMORY_GROWTH)
endif()

if(WIN32)
  target_link_libraries(engine PUBLIC Xinput)
endif()
