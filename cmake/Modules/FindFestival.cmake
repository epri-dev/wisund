set(_x86 "(x86)")
file(GLOB _Festival_INCLUDE_DIRS
    "$ENV{ProgramFiles}/festival/*/include"
    "$ENV{ProgramFiles${_x86}}/festival/*/include"
    )
unset(_x86)

find_path(FESTIVAL_INCLUDE_DIR
        NAMES
        festival.h
        PATHS
        ${_Festival_INCLUDE_DIRS}/festival
        /usr/include/festival
        /usr/local/include/festival
        ${FESTIVAL_ROOT}
)

find_path(SPEECH_TOOLS_INCLUDE_DIR
        NAMES
        siod.h
        PATHS
        ${_Festival_INCLUDE_DIRS}/../speech_tools
        /usr/include/speech_tools
        /usr/local/include/speech_tools
        ${FESTIVAL_ROOT}
)

# quick hack for now
set(SPEECH_TOOLS_LIB_DIR "/usr/lib/speech")
set(SPEECH_TOOLS_LIBRARY estbase estools eststring)
set(FESTIVAL_LIBRARY Festival)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
    Festival
    DEFAULT_MSG
    FESTIVAL_LIBRARY
    SPEECH_TOOLS_LIBRARY
    FESTIVAL_INCLUDE_DIR
    SPEECH_TOOLS_INCLUDE_DIR
)
