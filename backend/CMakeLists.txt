cmake_minimum_required(VERSION 3.10)
project(UltimateChess)

set(SOURCES
	src/main.cpp
	src/bitboard.cpp
	src/move_generation.cpp
	src/gamestate.cpp
 )
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

#if(MSVC)
 # add_compile_options(/O2 /W4)
#else()
  add_compile_options(-O3 -Wall -march=native)
#endif()

set(CMAKE_TOOLCHAIN_FILE "C:/Users/edwin/vcpkg/scripts/buildsystems/vcpkg.cmake")
set(crow_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/share/crow")
set(crow_INCLUDE_DIRS "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Boost_INCLUDE_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Boost_LIBRARY_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/lib")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)

#set(CMAKE_GENERATOR_TOOLSET "Ninja")
#set(CMAKE_GENERATOR_TOOLSET "v142" CACHE STRING "Platform Toolset" FORCE)

set(ASIO_INCLUDE_DIRS "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Asio_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/share/asio")



find_package(Asio REQUIRED)
find_package(crow CONFIG REQUIRED)


add_executable(UltimateChess ${SOURCES})
target_link_libraries(UltimateChess PRIVATE Crow::Crow)

add_definitions(-D_WIN32_WINNT=0x0601)
target_link_libraries(UltimateChess PRIVATE wsock32 ws2_32)
