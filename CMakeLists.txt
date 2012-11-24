cmake_minimum_required (VERSION 2.8)
project (StrEdit)

# Set some vars.
set (STREDIT_VERSION_MAJOR 1)
set (STREDIT_VERSION_MINOR 0)
set (STREDIT_VERSION_PATCH 0)

set (STREDIT_SRC "${CMAKE_SOURCE_DIR}/src")
set (STREDIT_LIBS boost_filesystem boost_system)
set (STREDIT_LIB_SRC "${STREDIT_SRC}/exception.cpp" "${STREDIT_SRC}/format.cpp" "${STREDIT_SRC}/helpers.cpp" "${STREDIT_SRC}/STREDITings.cpp")

# Set compiler flags.
set (CMAKE_C_FLAGS  "-m${STREDIT_ARCH}  -static -static-libstdc++ -static-libgcc")
set (CMAKE_CXX_FLAGS "-m${STREDIT_ARCH} -static -static-libstdc++ -static-libgcc")
set (CMAKE_EXE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
set (CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc")
set (CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc")

# Include source and library directories.
include_directories     ("${CMAKE_SOURCE_DIR}/externals/boost" "${CMAKE_SOURCE_DIR}/externals/utf8" ${STREDIT_SRC})
# Depending on platform and architecture, the libs are stored in different places.
IF (CMAKE_SYSTEM_NAME MATCHES "Windows" AND CMAKE_HOST_SYSTEM_NAME MATCHES "Linux")
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-mingw-${STREDIT_ARCH}/lib")
ELSE ()
    link_directories    ("${CMAKE_SOURCE_DIR}/externals/boost/stage-${STREDIT_ARCH}/lib")
ENDIF ()

# Build STREDITings library.
add_library         (strings ${STREDIT_LINK} ${STREDIT_LIB_SRC})
target_link_libraries   (strings ${STREDIT_LIBS})

# Build STREDITings tester.
add_executable          (STREDITings-tester ${STREDIT_SRC}/tester.cpp)
target_link_libraries   (STREDITings-tester strings)
