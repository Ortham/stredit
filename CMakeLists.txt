# Settings passed on the command line:
#
# STREDIT_LIBS_DIR = the directory which all external libraries may be referenced from.
# STREDIT_ARCH = the build architecture
# STREDIT_LINK = whether to build a static or dynamic library.

##############################
# General Settings
##############################

cmake_minimum_required (VERSION 2.8.9)
project (StrEdit)

set (STREDIT_SRC "${CMAKE_SOURCE_DIR}/src/progress.cpp" "${CMAKE_SOURCE_DIR}/src/backend.cpp" "${CMAKE_SOURCE_DIR}/src/ui.cpp" "${STREDIT_LIBS_DIR}/pugixml/src/pugixml.cpp")

# Include source and library directories.
include_directories ("${STREDIT_LIBS_DIR}/boost" "${STREDIT_LIBS_DIR}/pugixml/src" "${STREDIT_LIBS_DIR}/libstrings/src" "${CMAKE_SOURCE_DIR}/src")

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
    set (STREDIT_LIBS strings libboost_filesystem-vc110-mt-1_52 libboost_system-vc110-mt-1_52 libboost_locale-vc110-mt-1_52 wxmsw29u_core wxbase29u wxmsw29u_adv wxpng wxzlib comctl32 rpcrt4 shell32 gdi32 kernel32 user32 comdlg32 ole32 oleaut32 advapi32 msvcrt)
    set (CMAKE_CXX_FLAGS "/EHsc")
    set (CMAKE_EXE_LINKER_FLAGS "/SUBSYSTEM:WINDOWS")
    include_directories ("${STREDIT_LIBS_DIR}/wxWidgets/lib/vc_lib/mswu" "${CMAKE_SOURCE_DIR}/externals/wxWidgets/include")
    link_directories    ("${STREDIT_LIBS_DIR}/libstrings/build/Release" "${STREDIT_LIBS_DIR}/wxWidgets/lib/vc_lib")
ENDIF ()

# Settings when compiling and cross-compiling on Linux.
IF (CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    set (STREDIT_LIBS strings boost_filesystem boost_system boost_locale)
    set (CMAKE_C_FLAGS  "-m${STREDIT_ARCH}")
    set (CMAKE_CXX_FLAGS "-m${STREDIT_ARCH}")
    set (CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    set (CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
    link_directories    ("${STREDIT_LIBS_DIR}/libstrings/build")

    IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set (STREDIT_LIBS ${STREDIT_LIBS} wx_mswu_core-2.9-i586-mingw32msvc wx_baseu-2.9-i586-mingw32msvc wx_mswu_adv-2.9-i586-mingw32msvc wxpng-2.9-i586-mingw32msvc wxzlib-2.9-i586-mingw32msvc comctl32)
        set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--subsystem,windows")
        include_directories ("${STREDIT_LIBS_DIR}/wxWidgets/build-msw/lib/wx/include/i586-mingw32msvc-msw-unicode-static-2.9" "${STREDIT_LIBS_DIR}/wxWidgets/include")
        link_directories    ("${STREDIT_LIBS_DIR}/boost/stage-mingw-${STREDIT_ARCH}/lib" "${STREDIT_LIBS_DIR}/wxWidgets/build-msw/lib")
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
