# Desktop build of the tico frontend + fceumm, for developing and testing the
# Vulkan renderer and slang shaders off-device. The Switch NRO is built by
# build_fceumm_nro.sh. Run ./build_desktop.sh from the repo root.
cmake_minimum_required(VERSION 3.20)
project(tico_fceumm_desktop C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(ROOT ${CMAKE_CURRENT_SOURCE_DIR}/..)
set(DEPS ${CMAKE_CURRENT_SOURCE_DIR}/deps)
set(FCEUMM_CORE_LIB "" CACHE FILEPATH "Static fceumm libretro core")

set(ENABLE_OPT OFF CACHE BOOL "" FORCE)
set(ENABLE_HLSL OFF CACHE BOOL "" FORCE)
set(ENABLE_GLSLANG_BINARIES OFF CACHE BOOL "" FORCE)
set(GLSLANG_TESTS OFF CACHE BOOL "" FORCE)
set(BUILD_EXTERNAL OFF CACHE BOOL "" FORCE)
set(ENABLE_SPVREMAPPER OFF CACHE BOOL "" FORCE)
set(GLSLANG_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(${DEPS}/glslang glslang EXCLUDE_FROM_ALL)

find_package(PkgConfig REQUIRED)
pkg_check_modules(SDL2 REQUIRED IMPORTED_TARGET sdl2 SDL2_mixer)
find_package(CURL REQUIRED)
find_package(LibArchive REQUIRED)

file(GLOB RCHEEVOS_SOURCES ${ROOT}/rcheevos/src/*.c ${ROOT}/rcheevos/src/*/*.c)
list(FILTER RCHEEVOS_SOURCES EXCLUDE REGEX "rc_client_external\\.c$")

# the core archive carries libretro-common (VFS, file streams) itself
set(LRC_INCLUDE ${ROOT}/src/drivers/libretro/libretro-common/include)

add_library(rcheevos STATIC ${RCHEEVOS_SOURCES})
target_include_directories(rcheevos PUBLIC ${ROOT}/rcheevos/include PRIVATE ${LRC_INCLUDE})
target_compile_definitions(rcheevos PUBLIC RC_CLIENT_SUPPORTS_HASH)
# fceumm's src/md5.c has its own md5_finish, with another context struct
target_compile_definitions(rcheevos PRIVATE md5_finish=rc_md5_finish)

add_executable(tico-fceumm
    TicoMain.cpp TicoCore.cpp TicoVulkan.cpp TicoShaderChain.cpp TicoSlang.cpp
    overlay/imgui_overlay.cpp overlay/overlay_ui.cpp overlay/ra_alerts.cpp
    overlay/tico_config.cpp overlay/translation_manager.cpp
    ${DEPS}/volk/volk.c
    ${DEPS}/SPIRV-Reflect/spirv_reflect.c
    ${DEPS}/imgui/imgui.cpp ${DEPS}/imgui/imgui_draw.cpp ${DEPS}/imgui/imgui_tables.cpp
    ${DEPS}/imgui/imgui_widgets.cpp ${DEPS}/imgui/imgui_demo.cpp
    ${DEPS}/imgui/backends/imgui_impl_sdl2.cpp ${DEPS}/imgui/backends/imgui_impl_vulkan.cpp)
target_include_directories(tico-fceumm PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${DEPS} ${DEPS}/imgui ${DEPS}/imgui/backends
    ${DEPS}/volk ${DEPS}/vulkan-headers ${DEPS}/glslang ${DEPS}/SPIRV-Reflect
    ${LRC_INCLUDE})
target_compile_definitions(tico-fceumm PRIVATE
    IMGUI_IMPL_VULKAN_NO_PROTOTYPES VK_NO_PROTOTYPES TICO_APP_VERSION="desktop")
target_link_libraries(tico-fceumm PRIVATE
    ${FCEUMM_CORE_LIB} rcheevos glslang glslang-default-resource-limits
    PkgConfig::SDL2 CURL::libcurl LibArchive::LibArchive ${CMAKE_DL_LIBS} pthread)

# Headless preset runner: tools/shader_test.cpp
add_executable(shader_test
    tools/shader_test.cpp TicoVulkan.cpp TicoShaderChain.cpp TicoSlang.cpp
    ${DEPS}/volk/volk.c ${DEPS}/SPIRV-Reflect/spirv_reflect.c
    ${DEPS}/imgui/imgui.cpp ${DEPS}/imgui/imgui_draw.cpp ${DEPS}/imgui/imgui_tables.cpp
    ${DEPS}/imgui/imgui_widgets.cpp ${DEPS}/imgui/backends/imgui_impl_vulkan.cpp)
target_include_directories(shader_test PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR} ${DEPS} ${DEPS}/imgui ${DEPS}/imgui/backends
    ${DEPS}/volk ${DEPS}/vulkan-headers ${DEPS}/glslang ${DEPS}/SPIRV-Reflect ${LRC_INCLUDE})
target_compile_definitions(shader_test PRIVATE IMGUI_IMPL_VULKAN_NO_PROTOTYPES VK_NO_PROTOTYPES)
target_link_libraries(shader_test PRIVATE glslang glslang-default-resource-limits
    PkgConfig::SDL2 ${CMAKE_DL_LIBS} pthread)
