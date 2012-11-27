##############################
# General Settings
##############################

cmake_minimum_required (VERSION 2.8)
project (StrEdit)

# Set some vars.
set (StrEdit_VERSION_MAJOR 1)
set (StrEdit_VERSION_MINOR 0)
set (StrEdit_VERSION_PATCH 0)

set (STREDIT_SRC "${CMAKE_SOURCE_DIR}/src/backend.cpp" "${CMAKE_SOURCE_DIR}/src/ui.cpp")

# Include source and library directories.
include_directories     ("${CMAKE_SOURCE_DIR}/externals/boost" "${CMAKE_SOURCE_DIR}/externals/libstrings/src" "${CMAKE_SOURCE_DIR}/src")

##############################
# Platform-Specific Settings
##############################

# Settings when compiling for Windows.
IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
    add_definitions (-DUNICODE -D_UNICODE)
    set (STREDIT_SRC ${STREDIT_SRC} "${CMAKE_SOURCE_DIR}/resource.rc")
    IF (${STREDIT_LINK} MATCHES "STATIC")
        add_definitions (-DLIBSTRINGS_STATIC)
    ELSE ()
        add_definitions (-DLIBSTRINGS_EXPORT)
    ENDIF ()
ENDIF ()

# Settings when compiling on Windows.
IF (CMAKE_HOST_SYSTEM_NAME MATCHES "Windows")
    set (STREDIT_LIBS strings libboost_filesystem-vc110-mt-1_52 libboost_system-vc110-mt-1_52 libboost_locale-vc110-mt-1_52)
    set (CMAKE_CXX_FLAGS "/EHsc")
    set (CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS")
    set (STREDIT_LIBS ${STREDIT_LIBS} wxmsw29u_core wxbase29u wxmsw29u_adv wxpng wxzlib comctl32 rpcrt4.lib shell32.lib gdi32.lib kernel32.lib user32.lib comdlg32.lib ole32.lib oleaut32.lib advapi32.lib msvcrt.lib)
    include_directories ("${CMAKE_SOURCE_DIR}/externals/wxWidgets/lib/vc_lib/mswu" "${CMAKE_SOURCE_DIR}/externals/wxWidgets/include")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/libstrings/build/Release")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/wxWidgets/lib/vc_lib")
ENDIF ()

# Settings when compiling and cross-compiling on Linux.
IF (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set (LIBSTR_LIBS boost_filesystem boost_system)
    set (CMAKE_C_FLAGS  "-m${STREDIT_ARCH}  -static -static-libstdc++ -static-libgcc")
    set (CMAKE_CXX_FLAGS "-m${STREDIT_ARCH} -static -static-libstdc++ -static-libgcc")
    set (CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    set (CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/libstrings/build")

    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-mingw-${STREDIT_ARCH}/lib")
        set (STREDIT_LIBS ${STREDIT_LIBS} wx_mswu_core-2.9-i586-mingw32msvc wx_baseu-2.9-i586-mingw32msvc wx_mswu_adv-2.9-i586-mingw32msvc wxpng-2.9-i586-mingw32msvc wxzlib-2.9-i586-mingw32msvc comctl32)
        include_directories ("${CMAKE_SOURCE_DIR}/externals/wxWidgets/build-msw/lib/wx/include/i586-mingw32msvc-msw-unicode-static-2.9" "${CMAKE_SOURCE_DIR}/externals/wxWidgets/include")
        link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-mingw-${STREDIT_ARCH}/lib")
        link_directories    ("${CMAKE_SOURCE_DIR}/externals/wxWidgets/build-msw/lib")
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
        IF (STREDIT_ARCH MATCHES "32")
            set (MINGW i586-mingw32msvc)
        ELSE ()
            set (MINGW x86_64-w64-mingw32)
        ENDIF ()

        set (CMAKE_C_COMPILER   ${MINGW}-gcc)
        set (CMAKE_CXX_COMPILER ${MINGW}-g++)
        set (CMAKE_RC_COMPILER  ${MINGW}-windres)
        set (CMAKE_RANLIB       ${MINGW}-ranlib)
    ENDIF ()
ENDIF ()

# Settings when not cross-compiling.
IF (CMAKE_SYSTEM_NAME MATCHES CMAKE_HOST_SYSTEM_NAME)
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-${STREDIT_ARCH}/lib")
ENDIF ()

##############################
# Actual Building
##############################

add_executable          (StrEdit ${STREDIT_SRC})
target_link_libraries   (StrEdit ${STREDIT_LIBS})
