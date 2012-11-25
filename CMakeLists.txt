cmake_minimum_required (VERSION 2.8)
project (StrEdit)

# Set some vars.
set (StrEdit_VERSION_MAJOR 1)
set (StrEdit_VERSION_MINOR 0)
set (StrEdit_VERSION_PATCH 0)

set (STREDIT_SRC "${CMAKE_SOURCE_DIR}/src/backend.cpp" "${CMAKE_SOURCE_DIR}/src/ui.cpp")
set (STREDIT_LIBS   boost_locale boost_filesystem boost_system strings wx_baseu-2.9-i586-mingw32msvc wx_mswu_core-2.9-i586-mingw32msvc wx_mswu_adv-2.9-i586-mingw32msvc)
set (STREDIT_LIBS ${STREDIT_LIBS} comctl32 odbc32 wsock32 winspool winmm shell32 comctl32 ctl3d32 advapi32 wxpng-2.9-i586-mingw32msvc wxzlib-2.9-i586-mingw32msvc )

# Set compiler flags.
set (CMAKE_C_FLAGS  "-m${STREDIT_ARCH}  -static -static-libstdc++ -static-libgcc -O3")
set (CMAKE_CXX_FLAGS "-m${STREDIT_ARCH} -static -static-libstdc++ -static-libgcc -O3")
set (CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
set (CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

# Include source and library directories.
include_directories     ("${CMAKE_SOURCE_DIR}/externals/boost" "${CMAKE_SOURCE_DIR}/externals/libstrings/src" "${CMAKE_SOURCE_DIR}/externals/wxWidgets/include" "${CMAKE_SOURCE_DIR}/externals/wxWidgets/build-msw/lib/wx/include/i586-mingw32msvc-msw-unicode-static-2.9" "${CMAKE_SOURCE_DIR}/src")
# Depending on platform and architecture, the libs are stored in different places.
IF (CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-mingw-${STREDIT_ARCH}/lib")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/wxWidgets/build-msw/lib")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/libstrings/build")
ELSE ()
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-${STREDIT_ARCH}/lib")
ENDIF ()

# Build executable.
add_executable          (StrEdit ${STREDIT_SRC})
target_link_libraries   (StrEdit strings ${STREDIT_LIBS})
