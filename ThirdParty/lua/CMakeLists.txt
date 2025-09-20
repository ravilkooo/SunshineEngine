cmake_minimum_required(VERSION 3.16)

project(lua C)

set(LUA_SRC
    src/lapi.c
    src/lauxlib.c
    src/lbaselib.c
    src/lcode.c
    src/lcorolib.c
    src/lctype.c
    src/ldblib.c
    src/ldebug.c
    src/ldo.c
    src/ldump.c
    src/lfunc.c
    src/lgc.c
    src/linit.c
    src/liolib.c
    src/llex.c
    src/lmathlib.c
    src/lmem.c
    src/loadlib.c
    src/lobject.c
    src/lopcodes.c
    src/loslib.c
    src/lparser.c
    src/lstate.c
    src/lstring.c
    src/lstrlib.c
    src/ltable.c
    src/ltablib.c
    src/ltm.c
    # lua.c
    # luac.c
    src/lundump.c
    src/lutf8lib.c
    src/lvm.c
    src/lzio.c
)

add_library(lua STATIC ${LUA_SRC})

target_include_directories(lua PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Place in Solution Explorer under "ThirdPartyLibs"
set_target_properties(lua PROPERTIES FOLDER "ThirdPartyLibs")


#--------------------
# For VS
file(GLOB LUA_HEADERS "src/*.h" "src/*.hpp")

target_sources(lua PRIVATE ${LUA_HEADERS})

source_group("Header Files" FILES ${LUA_HEADERS})
