cmake_minimum_required(VERSION 3.14)

# Set the Qt6 installation path
set(CMAKE_PREFIX_PATH $ENV{QTDIR})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)


find_package(Qt6Core REQUIRED)
find_package(Qt6Widgets REQUIRED)

project(zxSpectrum)

set(SOURCES
    zxSpectrum.cpp
  
)

set(HEADERS

)

add_executable(zxSpectrum ${SOURCES} ${HEADERS})

target_link_libraries(zxSpectrum Qt6::Core Qt6::Widgets)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})