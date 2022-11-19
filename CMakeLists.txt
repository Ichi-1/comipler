cmake_minimum_required(VERSION 3.15)


find_program(CCACHE_PROGRAM ccache)

if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
endif()

# Export compilation DB in JSON format
set(CMAKE_EXPORT_COMPILE_COMMANDS on)


project(FUN_Compiler C)

add_executable(func src/main.c)
