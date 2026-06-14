cmake_minimum_required(VERSION 3.16)

project(SmartCopy LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# MinGW: statically link libs
if(MINGW)
    add_compile_options(-static-libgcc -static-libstdc++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -lpthread")
endif()

add_executable(SmartCopy main.cpp)

include(GNUInstallDirs)
install(TARGETS SmartCopy
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
