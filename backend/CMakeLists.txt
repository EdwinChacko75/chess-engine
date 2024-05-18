# cmake_minimum_required(VERSION 3.10)
# project(UltimateChess)

# set(SOURCES
#     src/main.cpp
#     src/bitboard.cpp
#     src/move_generation.cpp
#     src/gamestate.cpp
#     src/evaluate.cpp
#     src/move_pick.cpp
# )

# set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_STANDARD_REQUIRED True)

# include(CheckIPOSupported)
# check_ipo_supported(RESULT lto_supported)
# if(lto_supported)
#     set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
# endif()

# # Set optimization and warning flags
# if(MSVC)
#     add_compile_options(/O2 /W4)
# else()
#     add_compile_options(-O3 -Wall -march=native)
# endif()

# # Find Boost libraries
# find_package(Boost REQUIRED system filesystem thread regex program_options)

# # Manually add Asio and Crow include directory
# include_directories(/usr/include /usr/src/app/Crow/include)

# # Include directories
# include_directories(include ${Boost_INCLUDE_DIRS})

# # Add executable
# add_executable(UltimateChess ${SOURCES})

# # Link libraries
# target_link_libraries(UltimateChess PRIVATE Boost::system Boost::filesystem Boost::thread Boost::regex Boost::program_options pthread)



cmake_minimum_required(VERSION 3.10)
project(UltimateChess)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Sources
set(SOURCES
    src/main.cpp
    src/bitboard.cpp
    src/move_generation.cpp
    src/gamestate.cpp
    src/evaluate.cpp 
    src/move_pick.cpp "src/transposition_table.cpp")

# Interprocedural Optimization (LTO)
include(CheckIPOSupported)
check_ipo_supported(RESULT lto_supported)
if(lto_supported)
  set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif()

# Compilation flags
if(MSVC)
  add_compile_options(/W4) # Warning level 4 for MSVC
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(/RTC1)
  else()
    add_compile_options(/O2)
  endif()
else()
  add_compile_options(-Wall -march=native)
  if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(-O0 -g)
  else()
    add_compile_options(-O3)
  endif()
endif()

# Vcpkg toolchain
set(CMAKE_TOOLCHAIN_FILE "C:/Users/edwin/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Dependencies paths
set(crow_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/share/crow")
set(crow_INCLUDE_DIRS "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Boost_INCLUDE_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Boost_LIBRARY_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/lib")
set(ASIO_INCLUDE_DIRS "C:/Users/edwin/vcpkg/installed/x64-windows/include")
set(Asio_DIR "C:/Users/edwin/vcpkg/installed/x64-windows/share/asio")

# Output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find packages
find_package(Asio REQUIRED)
find_package(crow CONFIG REQUIRED)

# Add executable
add_executable(UltimateChess ${SOURCES})

# Link libraries
target_link_libraries(UltimateChess PRIVATE Crow::Crow)
add_definitions(-D_WIN32_WINNT=0x0601)
target_link_libraries(UltimateChess PRIVATE wsock32 ws2_32)

