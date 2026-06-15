cmake_minimum_required(VERSION 3.16)

project(AutoBuilder LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(AutoBuilder main.cpp
    decap/decap.cpp decap/decap.h decap/dic.cpp decap/exceptions.cpp
    egyptian.cpp egyptian.h entities.cpp entities.h forget.cpp forget.h legacy.cpp legacy.h
    library.cpp library.h linebreak.cpp linebreak.h loader.cpp loader.h main.cpp
    stringlib.cpp stringlib.h sutton.cpp sutton.h textbase.cpp textbase.h ucdcom.cpp ucdcom.h
    unibase.cpp unibase.h
    ../Libs/PugiXml/pugiconfig.hpp ../Libs/PugiXml/pugixml.cpp ../Libs/PugiXml/pugixml.hpp
    ../Unicodia/Uc/UcFlags.h
    ../Libs/SelfMade/Strings/u_EgypIndex.cpp ../Libs/SelfMade/Strings/u_EgypIndex.h
    ../Libs/SelfMade/Strings/u_Strings.cpp ../Libs/SelfMade/Strings/u_Strings.h)

include_directories(${CMAKE_SOURCE_DIR}/../Libs)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/PugiXml)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/RapidJson)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/Mojibake)
include_directories(${CMAKE_SOURCE_DIR}/../Libs/SelfMade/Strings)
include_directories(${CMAKE_SOURCE_DIR}/../Unicodia/Uc)

# MinGW: statically link libs
if(MINGW)
    add_compile_options(-static-libgcc -static-libstdc++)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static -lpthread")
endif()

include(GNUInstallDirs)
install(TARGETS AutoBuilder
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
