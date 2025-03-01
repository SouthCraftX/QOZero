project(QuickOK-Zero VERSION 0.0.1-Alpha)

cmake_minimum_required(VERSION 3.15)

include(FetchContent)

set(${FETCHCONTENT_BASE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/build_temp/downloads)

FetchContent_Declare(
    mimalloc
    GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
    GIT_TAG v2.1.1
)

FetchContent_MakeAvailable(mimalloc)

FetchContent_Declare(
    StringZilla
    GIT_REPOSITORY https://github.com/ashvardanian/StringZilla
    GIT_TAG v3.12.1
)

FetchContent_MakeAvailable(StringZilla)

if(NOT DEFINED QOZ_INSTALL_DIR)
    set(QOZ_INSTALL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/install)
endif()

add_library(QOZ_DEPS_INTERFACE INTERFACE)
target_include_directories(QOZ_DEPS_INTERFACE INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(QOZ_DEPS_INTERFACE INTERFACE mimalloc StringZilla)

add_library(QOZ_DEPS_STATIC STATIC dummy_source.c)
target_link_libraries(QOZ_DEPS_STATIC INTERFACE QOZ_DEPS_INTERFACE)

add_library(QOZ_DEPS_SHARED SHARED dummy_source.c)
target_link_libraries(QOZ_DEPS_SHARED INTERFACE QOZ_DEPS_INTERFACE)

install(TARGETS QOZ_DEPS_STATIC QOZ_DEPS_SHARED
        EXPORT QOZ_TARGERS
        ARCHIVE DESTINATION ${QOZ_INSTALL_DIR}/lib
        LIBRARY DESTINATION ${QOZ_INSTALL_DIR}/lib)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${QOZ_INSTALL_DIR}/include)