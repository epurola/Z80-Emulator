cmake_minimum_required(VERSION 3.14)

set (This z80Emulator)

project(z80Emulator LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_POSITION_INDEPENDENT_CODE)
set(CMAKE_BUILD_TYPE Debug)

if (MSVC)

    add_compile_options(/Zc:__cplusplus)
    
    add_compile_options(/permissive-)
endif()

enable_testing()


add_subdirectory(googletest)


set(Headers
      main.hpp
      Instruction.hpp
      Memory.hpp
      Bus.hpp
      IODevice.hpp
)
set(Sources
      main.cpp
      Instruction.cpp
      Memory.cpp
      Bus.cpp
      IODevice.cpp
)

add_library(${This} STATIC ${Sources} ${Headers})

add_subdirectory(test)
add_subdirectory(DebuggerQT)
add_subdirectory(fuseTest)













