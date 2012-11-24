# Cross-compiling from Linux to Windows.
set (CMAKE_SYSTEM_NAME Windows)

IF (STREDIT_ARCH MATCHES "32")
    set (MINGW i586-mingw32msvc)
ELSE ()
    set (MINGW x86_64-w64-mingw32)
ENDIF ()

set (CMAKE_C_COMPILER   ${MINGW}-gcc)
set (CMAKE_CXX_COMPILER ${MINGW}-g++)
set (CMAKE_RC_COMPILER  ${MINGW}-windres)
set (CMAKE_RANLIB       ${MINGW}-ranlib)
#set (CMAKE_FORCE_AR           ${MINGW}-ar)

IF (${STREDIT_LINK} MATCHES "STATIC")
    add_definitions (-DLIBSTRINGS_STATIC)
ELSE ()
    add_definitions (-DLIBSTRINGS_EXPORT)
ENDIF ()
