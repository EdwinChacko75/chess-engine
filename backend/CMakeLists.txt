cmake_minimum_required(VERSION 3.10)
project(UltimateChess)

set(SOURCES
    src/main.cpp
    src/bitboard.cpp
    src/move_generation.cpp
    src/gamestate.cpp
    src/evaluate.cpp
    src/move_pick.cpp
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Set optimization and warning flags
if(MSVC)
    add_compile_options(/O2 /W4)
else()
    add_compile_options(-O3 -Wall -march=native)
endif()

# Find Boost libraries
find_package(Boost REQUIRED system filesystem thread regex program_options)

# Manually add Asio and Crow include directory
include_directories(/usr/include /usr/src/app/Crow/include)

# Include directories
include_directories(include ${Boost_INCLUDE_DIRS})

# Add executable
add_executable(UltimateChess ${SOURCES})

# Link libraries
target_link_libraries(UltimateChess PRIVATE Boost::system Boost::filesystem Boost::thread Boost::regex Boost::program_options pthread)
