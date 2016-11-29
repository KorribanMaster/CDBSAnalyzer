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

FIND_PACKAGE(Qt5 COMPONENTS Core Network Concurrent DBus Widgets Gui PrintSupport REQUIRED)
LINK_LIBRARIES(Qt5::Core Qt5::Network Qt5::DBus Qt5::Widgets Qt5::Gui Qt5::PrintSupport)
INCLUDE_DIRECTORIES(${Qt5Core_INCLUDE_DIRS} ${Qt5Network_INCLUDE_DIRS} ${Qt5Concurrent_INCLUDE_DIRS} ${Qt5PrintSupport_INCLUDE_DIRS})


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
#-Wall -Wno-long-long -pedantic
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -g -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# LIBS
INCLUDE_DIRECTORIES(libs
    libs/pixel
    libs/hist
    gui
    qcustomplot
    
)

ADD_LIBRARY(abstractpixel SHARED libs/pixel/abstractpixel.cpp)
target_link_libraries(abstractpixel)

add_library(cdbpixel SHARED libs/pixel/cdbpixel.cpp)
target_link_libraries(cdbpixel abstractpixel)

add_library(roipixel SHARED libs/pixel/roipixel.cpp)
target_link_libraries(roipixel abstractpixel)

add_library(mpahistfile SHARED libs/hist/mpahistfile.cpp)
target_link_libraries(mpahistfile mpa1dhist mpa2dhist)

add_library(mpa1dhist SHARED libs/hist/mpa1dhist.cpp)
target_link_libraries(mpa1dhist abstractpixel roipixel cdbpixel)

add_library(mpa2dhist SHARED libs/hist/mpa2dhist.cpp)
target_link_libraries(mpa2dhist abstractpixel roipixel cdbpixel)

add_library(qcustomplot SHARED qcustomplot/qcustomplot.cpp)
target_link_libraries(qcustomplot)


#GUI
#set(gui_files gui/*.ui)
#source_group(gui FILES ${gui_files})
#QT5_WRAP_UI(guis_h ${gui_files})

#set_property(${gui_files} DIRECTORY gui)

add_library(mainwindow SHARED gui/mainwindow.cpp)
target_link_libraries(mainwindow Qt5::Widgets mpahistfile abstractpixel)

add_library(plotwindow SHARED gui/plotwindow.cpp)
target_link_libraries(plotwindow mpa1dhist mpa2dhist qcustomplot)


INSTALL(TARGETS abstractpixel cdbpixel roipixel
    LIBRARY DESTINATION lib
)



# APPS
ADD_EXECUTABLE(cdbanalyzer main.cpp)
TARGET_LINK_LIBRARIES(cdbanalyzer mainwindow abstractpixel )

INSTALL(TARGETS cdbanalyzer
    RUNTIME DESTINATION bin
)

#TESTS
add_executable(pixeltest test/pixeltest.cpp)
target_link_libraries(pixeltest abstractpixel roipixel cdbpixel)

add_executable(maptest test/maptest.cpp)
target_link_libraries(maptest cdbpixel roipixel abstractpixel)

add_executable(hist2dtest test/hist2dtest.cpp)
target_link_libraries(hist2dtest mpa2dhist mpa1dhist cdbpixel roipixel abstractpixel ${MATHGL2_LIBRARIES})

add_executable(plottest test/plottest.cpp)
target_link_libraries(plottest plotwindow)

install(TARGETS pixeltest maptest RUNTIME DESTINATION bin)
