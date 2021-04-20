cmake_minimum_required(VERSION 3.10)
project(cpp_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE "Debug" CACHE STRING "Set the build type." FORCE)

set(CMAKE_VERBOSE_MAKEFILE true)
set(CMAKE_EXPORT_COMPILE_COMMANDS true)

include_directories(include)

set(APP_SOURCE ttt.cpp) 

find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIRS})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lncurses")
add_executable(tictactoe ${APP_SOURCE})
install(TARGETS tictactoe DESTINATION bin)
target_link_libraries(tictactoe curses)

# install demo script
#install(PROGRAMS demo DESTINATION bin)
#install(FILES autoTTT.txt DESTINATION bin)
