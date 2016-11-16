CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(CDBSAnalyzer)
SET(CDBSAnalyzer_VERSION_STRING 0.1.0)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/version.h.in ${PROJECT_BINARY_DIR}/version.h @ONLY)
MESSAGE("CDB Analyzer")
MESSAGE("Eicke Hecht")


OPTION(WITH_ROOT "Build ROOT dependent apps" OFF)
OPTION(WITH_PYTHON2 "Build Python2 modules" OFF)
OPTION(WITH_PYTHON3 "Build Python3 modules (overrides WITH_PYTHON2)" ON)
option(WITH_MGL2 "Build mgl2 dependent gui" ON)

SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
SET(CMAKE_INSTALL_BINDIR bin)
SET(CMAKE_INSTALL_LIBDIR lib)
SET(CMAKE_INCLUDE_CURRENT_DIR ON)
SET(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

FIND_PACKAGE(Qt5 COMPONENTS Core Network Concurrent DBus Widgets Gui REQUIRED)
LINK_LIBRARIES(Qt5::Core Qt5::Network Qt5::DBus Qt5::Widgets Qt5::Gui)
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS})


find_package(Eigen3 REQUIRED)
include_directories(${EIGEN3_INCLUDE_DIR})

if(WITH_MGL2)
    find_package(MathGL2 REQUIRED)
    include_directories(${MATHGL2_INCLUDE_DIRS})
    #   add_executable(myexe main.cpp)
    #   target_link_libraries(myexe ${MATHGL2_LIBRARIES})
endif()

IF(WITH_PYTHON2 OR WITH_PYTHON3)
    IF(WITH_PYTHON3)
        FIND_PACKAGE(PythonLibs 3 REQUIRED)
        STRING(REGEX REPLACE "[^0-9]" "" boost_py_version ${PYTHONLIBS_VERSION_STRING})
    ELSE()
        FIND_PACKAGE(PythonLibs 2.6 REQUIRED)       
    ENDIF()
ENDIF()

IF(WITH_ROOT)
    MESSAGE(STATUS "Building with ROOT depencencies")
    ADD_DEFINITIONS(-DBUILD_WITH_ROOT)
    FIND_PACKAGE(ROOT 5.34.30 REQUIRED COMPONENTS Hist Tree RHTTP Graf Gui)
    SET(MY_ROOT_LIBRARIES Hist Tree)
    SET(EICKE_ROOT_LIBRARIES Graf Gui)
    INCLUDE_DIRECTORIES(${ROOT_INCLUDE_DIRS})
ENDIF()



SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -Wall -Wno-long-long -pedantic -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# LIBS
INCLUDE_DIRECTORIES(libs
    libs/pixel
    libs/hist
    gui
    
)

ADD_LIBRARY(abstractpixel SHARED libs/pixel/abstractpixel.cpp)

add_library(cdbpixel SHARED libs/pixel/cdbpixel.cpp)
target_link_libraries(cdbpixel abstractpixel)

add_library(roipixel SHARED libs/pixel/roipixel.cpp)
target_link_libraries(roipixel abstractpixel)

add_library(mpahistfile SHARED libs/hist/mpahistfile.cpp)
target_link_libraries(mpahistfile)

add_library(mpa1dhist SHARED libs/hist/mpa1dhist.cpp)
target_link_libraries(mpa1dhist)

add_library(mpa2dhist SHARED libs/hist/mpa2dhist.cpp)
target_link_libraries(mpa2dhist)


#GUI

add_library(mainwindow SHARED gui/mainwindow.cpp)
target_link_libraries(mainwindow Qt5::Widgets)




INSTALL(TARGETS abstractpixel cdbpixel roipixel
    LIBRARY DESTINATION lib
)



# APPS
ADD_EXECUTABLE(cdbanalyzer main.cpp)
TARGET_LINK_LIBRARIES(cdbanalyzer mainwindow)


INSTALL(TARGETS cdbanalyzer
    RUNTIME DESTINATION bin
)


