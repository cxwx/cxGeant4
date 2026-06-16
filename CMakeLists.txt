cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

file(READ "VERSION.md" ver)
string(REGEX MATCH "MAJ ([0-9]*)" _ ${ver})
set(CX_VER_MAJ ${CMAKE_MATCH_1}) # major
string(REGEX MATCH "MIN ([0-9]*)" _ ${ver})
set(CX_VER_MIN ${CMAKE_MATCH_1}) # minor
string(REGEX MATCH "PAT ([0-9]*)" _ ${ver})
set(CX_VER_PAT ${CMAKE_MATCH_1}) # patch
set(CX_VER_SS ${CX_VER_MAJ}.${CX_VER_MIN}.${CX_VER_PAT})
message("Version: ${CX_VER_SS}")

project(
    cxg4
    VERSION ${CX_VER_SS}
    DESCRIPTION "g4 tools made by cx"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_MACOSX_RPATH 1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH ON)

option(CXG4PLOT "geant4 vis" ON)
if(CXG4PLOT)
    find_package(Geant4 11.2 REQUIRED ui_all vis_all)
else()
    find_package(Geant4 11.2 REQUIRED)
endif()

find_package(spdlog REQUIRED)
find_package(yaml-cpp REQUIRED)

set(CMAKE_INSTALL_PREFIX $ENV{HOME}/software/install/)

include_directories(
    ${PROJECT_SOURCE_DIR}/
    ${Geant4_INCLUDE_DIR}/
    /opt/homebrew/include
    /usr/include/
    /usr/local/include/
    $ENV{HOME}/software/install/include
)

link_directories(
    /usr/lib/
    /usr/local/lib
    /opt/homebrew/lib
    $ENV{HOME}/software/install/lib
)
file(GLOB sources ${PROJECT_SOURCE_DIR}/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/*.hh)
set(LINK_LIBS spdlog::spdlog yaml-cpp::yaml-cpp ${Geant4_LIBRARIES})

add_library(cxg4 SHARED ${sources} ${headers})
target_link_libraries(cxg4 ${LINK_LIBS})
set_target_properties(
    cxg4
    PROPERTIES VERSION ${CX_VER_SS} SOVERSION ${CX_VER_MAJ}
)

add_executable(cxg4_materialprint cxg4_materialprint.cpp)
target_link_libraries(cxg4_materialprint ${Geant4_LIBRARIES})
add_executable(cxg4_DumpUnit cxg4_DumpUnit.cpp)
target_link_libraries(cxg4_DumpUnit ${Geant4_LIBRARIES})
add_executable(cxg4_DumpParticle cxg4_DumpParticle.cpp)
target_link_libraries(cxg4_DumpParticle cxg4 ${LINK_LIBS})
add_executable(cxg4_DumpPhysics cxg4_DumpPhysics.cpp)
target_link_libraries(cxg4_DumpPhysics cxg4 ${LINK_LIBS})

install(FILES cxGeant4.hh DESTINATION include/cxfunc)
install(
    TARGETS
        cxg4
        cxg4_materialprint
        cxg4_DumpUnit
        cxg4_DumpPhysics
        cxg4_DumpParticle
    EXPORT ${PROJECT_NAME}Targets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION ${CX_INSTALL_LIBS}
    ARCHIVE DESTINATION ${CX_INSTALL_LIBS}
)
