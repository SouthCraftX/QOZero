# --- START OF QOZero/CMakeLists.txt ---
cmake_minimum_required(VERSION 3.15)
project(QOZero VERSION 0.0.1)

include(FetchContent)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

option(QOZ_BUNDLE_DEPS "Bundles the dependencies within the library" ON)

# --- Define Link Type Option ---
if(QOZ_BUNDLE_DEPS)
    set(QOZ_BUNDLED_DEPS_LINK_TYPE "STATIC" CACHE STRING
        "Link type for bundled mimalloc & StringZilla SHARED library: STATIC, SHARED, BOTH")
    set_property(CACHE QOZ_BUNDLED_DEPS_LINK_TYPE PROPERTY STRINGS STATIC SHARED BOTH)
    message(STATUS "QOZero Configuration:")
    message(STATUS "  Bundle Dependencies (QOZ_BUNDLE_DEPS): ${QOZ_BUNDLE_DEPS}")
    message(STATUS "  Requested Link Type for Bundled Dependencies: ${QOZ_BUNDLED_DEPS_LINK_TYPE}")

    # --- Explain StringZilla Behavior ---
    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "STATIC")
        message(STATUS "    - StringZilla: Will be configured for HEADER-ONLY usage (no library built).")
    else() # SHARED or BOTH
        message(STATUS "    - StringZilla: Will be configured to build SHARED library (stringzilla_shared).")
    endif()

    # --- Explain Mimalloc Behavior ---
    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "STATIC")
        message(STATUS "    - Mimalloc: Will be configured to build STATIC library (mimalloc-static).")
    elseif(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "SHARED")
        message(STATUS "    - Mimalloc: Will be configured to build SHARED library (mimalloc).")
    else() # BOTH
        message(STATUS "    - Mimalloc: Will be configured to build BOTH static (mimalloc-static) and shared (mimalloc).")
    endif()
endif()

if(QOZ_BUNDLE_DEPS)
    # --- StringZilla Configuration ---
    set(SZ_ARGS "-DSTRINGZILLA_BUILD_TESTS=OFF")
    set(SZ_BUILD_SHARED_FLAG "OFF") # Default to OFF

    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "SHARED" OR QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH")
        set(SZ_BUILD_SHARED_FLAG "ON") # Build shared if SHARED or BOTH requested
    endif()

    list(APPEND SZ_ARGS "-DSTRINGZILLA_BUILD_SHARED=${SZ_BUILD_SHARED_FLAG}")
    message(STATUS "  Configuring StringZilla with CMAKE_ARGS: ${SZ_ARGS}")

    # --- Mimalloc Configuration ---
    set(MI_ARGS "-DMI_BUILD_TESTS=OFF -DMI_OVERRIDE=ON -DMI_INSTALL_TOPLEVEL=OFF")
    set(MI_BUILD_STATIC_ARG "")
    set(MI_BUILD_SHARED_ARG "")

    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "STATIC")
        set(MI_BUILD_STATIC_ARG "-DMI_BUILD_STATIC=ON")
        set(MI_BUILD_SHARED_ARG "-DMI_BUILD_SHARED=OFF")
    elseif(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "SHARED")
        set(MI_BUILD_STATIC_ARG "-DMI_BUILD_STATIC=OFF")
        set(MI_BUILD_SHARED_ARG "-DMI_BUILD_SHARED=ON")
    elseif(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH")
        set(MI_BUILD_STATIC_ARG "-DMI_BUILD_STATIC=ON")
        set(MI_BUILD_SHARED_ARG "-DMI_BUILD_SHARED=ON")
    endif()

    list(APPEND MI_ARGS ${MI_BUILD_STATIC_ARG} ${MI_BUILD_SHARED_ARG})
    message(STATUS "  Configuring Mimalloc with CMAKE_ARGS: ${MI_ARGS}")

    # --- Declare and Make Available ---
    FetchContent_Declare(
        mimalloc
        GIT_REPOSITORY https://github.com/microsoft/mimalloc.git
        GIT_TAG v2.1.1 # Consider updating tag
        CMAKE_ARGS ${MI_ARGS}
    )
    FetchContent_Declare(
        stringzilla
        GIT_REPOSITORY https://github.com/ashvardanian/StringZilla
        GIT_TAG v3.12.5
        CMAKE_ARGS ${SZ_ARGS}
    )
    FetchContent_MakeAvailable(stringzilla mimalloc)

    # --- *** FIX: Prevent Subdirectories from Installing Themselves *** ---
    # Since QOZero handles the installation of bundled dependencies via
    # install(TARGETS ...) and explicit install(FILES ...), disable the
    # installation rules within the dependencies themselves to avoid conflicts
    # like the mimalloc-redirect.dll error.
    set_directory_properties(PROPERTIES EXCLUDE_FROM_ALL ON) # Maybe helps build speed slightly
    # Get the binary directories FetchContent created
    FetchContent_GetProperties(stringzilla SOURCE_DIR STRINGZILLA_SOURCE_DIR BINARY_DIR STRINGZILLA_BINARY_DIR)
    FetchContent_GetProperties(mimalloc SOURCE_DIR MIMALLOC_SOURCE_DIR BINARY_DIR MIMALLOC_BINARY_DIR)
    # Explicitly disable installation from the subproject's build directory
    if(EXISTS "${STRINGZILLA_BINARY_DIR}")
        set_property(DIRECTORY ${STRINGZILLA_BINARY_DIR} PROPERTY EXCLUDE_FROM_INSTALL ON)
        message(STATUS "  Disabling install rules from StringZilla subdirectory: ${STRINGZILLA_BINARY_DIR}")
    endif()
    if(EXISTS "${MIMALLOC_BINARY_DIR}")
        set_property(DIRECTORY ${MIMALLOC_BINARY_DIR} PROPERTY EXCLUDE_FROM_INSTALL ON)
        message(STATUS "  Disabling install rules from Mimalloc subdirectory: ${MIMALLOC_BINARY_DIR}")
    endif()
    # --- End of FIX ---

    # --- Find Expected Targets ---
    set(SZ_STATIC_TARGET "") # No static library target exists for stringzilla
    set(SZ_SHARED_TARGET "") # Look for 'stringzilla_shared' if built
    set(MI_STATIC_TARGET "")
    set(MI_SHARED_TARGET "")

    # Find StringZilla SHARED target only if requested
    if(SZ_BUILD_SHARED_FLAG STREQUAL "ON")
        if(TARGET stringzilla_shared)
            set(SZ_SHARED_TARGET stringzilla_shared)
        else()
            message(WARNING "Expected stringzilla_shared target not found, although STRINGZILLA_BUILD_SHARED=ON was passed!")
        endif()
    endif()

    # Find mimalloc targets
    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "STATIC" OR QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH")
        if(TARGET mimalloc-static)
            set(MI_STATIC_TARGET mimalloc-static)
        endif()
    endif()

    if(QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "SHARED" OR QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH")
        if(TARGET mimalloc)
            set(MI_SHARED_TARGET mimalloc)
        elseif(TARGET mimalloc-shared)
            set(MI_SHARED_TARGET mimalloc-shared)
        endif()
    endif()

    # --- Explicit install for mimalloc redirect files (if shared mimalloc found) ---
    if(MI_SHARED_TARGET)
        set(MIMALLOC_AUX_SRC_DIR "${CMAKE_CURRENT_BINARY_DIR}/_deps/mimalloc-src/bin")
        set(MIMALLOC_REDIRECT_DLL "${MIMALLOC_AUX_SRC_DIR}/mimalloc-redirect.dll")
        set(MIMALLOC_REDIRECT_LIB "${MIMALLOC_AUX_SRC_DIR}/mimalloc-redirect.lib")
        install(FILES ${MIMALLOC_REDIRECT_DLL} DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT Runtime OPTIONAL)
        install(FILES ${MIMALLOC_REDIRECT_LIB} DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT Development OPTIONAL)
    endif()

    # --- Report Found Targets ---
    message(STATUS "  Found Targets (based on requested type '${QOZ_BUNDLED_DEPS_LINK_TYPE}'):")
    message(STATUS "    Stringzilla Static: Not Applicable (no static library target)")

    if(SZ_SHARED_TARGET)
        message(STATUS "    Stringzilla Shared: ${SZ_SHARED_TARGET}")
    else()
        message(STATUS "    Stringzilla Shared: Not Found/Expected")
    endif()

    if(MI_STATIC_TARGET)
        message(STATUS "    Mimalloc Static:    ${MI_STATIC_TARGET}")
    else()
        message(STATUS "    Mimalloc Static:    Not Found/Expected")
    endif()

    if(MI_SHARED_TARGET)
        message(STATUS "    Mimalloc Shared:    ${MI_SHARED_TARGET}")
    else()
        message(STATUS "    Mimalloc Shared:    Not Found/Expected")
    endif()

    # --- Error checking ---
    # StringZilla: Warn if shared was requested but not found
    if(SZ_BUILD_SHARED_FLAG STREQUAL "ON" AND NOT SZ_SHARED_TARGET)
        message(WARNING "Requested SHARED build for StringZilla, but 'stringzilla_shared' target not found!")
    endif()

    # Mimalloc checks remain strict
    if((QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "STATIC" OR QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH") AND NOT MI_STATIC_TARGET)
        message(FATAL_ERROR "Requested STATIC build for mimalloc, but target ('mimalloc-static') not found!")
    endif()

    if((QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "SHARED" OR QOZ_BUNDLED_DEPS_LINK_TYPE STREQUAL "BOTH") AND NOT MI_SHARED_TARGET)
        message(FATAL_ERROR "Requested SHARED build for mimalloc, but target ('mimalloc' or 'mimalloc-shared') not found!")
    endif()
else() # if not QOZ_BUNDLE_DEPS
    message(STATUS "Skipping bundling of dependencies. Downstream projects must provide stringzilla and mimalloc.")
endif()

# --- Define QOZero INTERFACE library and Selectors ---
add_library(QOZero INTERFACE)
target_include_directories(QOZero INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# Always add stringzilla INTERFACE target for headers, regardless of linking type
# Note: stringzilla INTERFACE target is created by its CMakeLists.txt
if(QOZ_BUNDLE_DEPS)
    if(TARGET stringzilla) # The base INTERFACE target from stringzilla
        target_link_libraries(QOZero INTERFACE stringzilla)
    endif()
endif()

add_library(QOZero_CoreStatic INTERFACE)
add_library(QOZero_CoreShared INTERFACE)
target_link_libraries(QOZero_CoreStatic INTERFACE QOZero) # Inherit base QOZero includes
target_link_libraries(QOZero_CoreShared INTERFACE QOZero) # Inherit base QOZero includes

# Link dependencies based on found targets
if(QOZ_BUNDLE_DEPS)
    # CoreStatic links ONLY mimalloc static (stringzilla doesn't provide static lib)
    if(MI_STATIC_TARGET)
        target_link_libraries(QOZero_CoreStatic INTERFACE ${MI_STATIC_TARGET})
    endif()

    # CoreShared links stringzilla_shared (if built) and mimalloc shared
    if(SZ_SHARED_TARGET)
        target_link_libraries(QOZero_CoreShared INTERFACE ${SZ_SHARED_TARGET})
    endif()

    if(MI_SHARED_TARGET)
        target_link_libraries(QOZero_CoreShared INTERFACE ${MI_SHARED_TARGET})
    endif()

    message(STATUS "Configured internal targets QOZero_CoreStatic and QOZero_CoreShared based on bundled type '${QOZ_BUNDLED_DEPS_LINK_TYPE}'.")
else()
    message(STATUS "Configured internal targets QOZero_CoreStatic and QOZero_CoreShared (unbundled). Linkage determined downstream.")
endif()

# --- Create ALIAS targets ---
add_library(QOZero::CoreStatic ALIAS QOZero_CoreStatic)
add_library(QOZero::CoreShared ALIAS QOZero_CoreShared)
message(STATUS "Created ALIAS targets QOZero::CoreStatic and QOZero::CoreShared.")

# --- Installation ---
# (Install Headers logic remains the same)
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/ DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/qozero" FILES_MATCHING PATTERN "*.h")

# (Prepare CMake Package Config Files logic remains the same)
set(QOZ_CONFIG_INSTALL_DIR "${CMAKE_INSTALL_DATADIR}/cmake/QOZero")

# ... (write_basic_package_version_file, configure_package_config_file, install(FILES ...)) ...

# Export and Install Targets
set(ALL_TARGETS_FOR_EXPORT "")

if(QOZ_BUNDLE_DEPS)
    # Only add ACTUAL library targets that were found
    if(SZ_SHARED_TARGET)
        list(APPEND ALL_TARGETS_FOR_EXPORT ${SZ_SHARED_TARGET})
    endif() # Only shared possible

    if(MI_STATIC_TARGET)
        list(APPEND ALL_TARGETS_FOR_EXPORT ${MI_STATIC_TARGET})
    endif()

    if(MI_SHARED_TARGET)
        list(APPEND ALL_TARGETS_FOR_EXPORT ${MI_SHARED_TARGET})
    endif()
endif()

# Always add interface targets to the list for the install command
# *** FIX: Also add the base 'stringzilla' INTERFACE target if bundling ***
if(QOZ_BUNDLE_DEPS AND TARGET stringzilla)
    list(APPEND ALL_TARGETS_FOR_EXPORT stringzilla)
endif()

list(APPEND ALL_TARGETS_FOR_EXPORT QOZero QOZero_CoreStatic QOZero_CoreShared)
list(REMOVE_DUPLICATES ALL_TARGETS_FOR_EXPORT)

if(ALL_TARGETS_FOR_EXPORT)
    message(STATUS "Adding targets to export set: ${ALL_TARGETS_FOR_EXPORT}")
    install(TARGETS ${ALL_TARGETS_FOR_EXPORT}
        EXPORT QOZeroTargets
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    )
else()
    message(WARNING "No targets identified for export set.")
endif()

# Install the Export Set File
install(EXPORT QOZeroTargets
    FILE QOZeroTargets.cmake
    NAMESPACE QOZero:: # Note: This applies namespace to exported targets like mimalloc, stringzilla_shared too if they aren't ALIASES.

    # This is usually OK. Downstream links QOZero::CoreStatic/Shared.
    DESTINATION ${QOZ_CONFIG_INSTALL_DIR}
)

# --- END OF QOZero/CMakeLists.txt ---