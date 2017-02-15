CMAKE_MINIMUM_REQUIRED(VERSION 2.8.11)

PROJECT(CDBSAnalyzer)
SET(CDBSAnalyzer_VERSION_STRING 0.1.0)
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/version.h.in ${PROJECT_BINARY_DIR}/version.h @ONLY)
MESSAGE("CDB Analyzer")
MESSAGE("Eicke Hecht")


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

SET(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#-Wall -Wno-long-long -pedantic -g
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fPIC")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# LIBS
INCLUDE_DIRECTORIES(libs
    libs/pixel
    libs/hist
    gui
    gui/tables
    gui/widgets
    #qcustomplot
    qcustomplot2
    
)

add_library(qcustomplot SHARED qcustomplot2/qcustomplot.cpp)
target_link_libraries(qcustomplot)

add_library(hist SHARED
    libs/hist/mpa1dhist.cpp
    libs/hist/mpa2dhist.cpp
    libs/hist/mpacdbhist.cpp
    libs/hist/mparefhist.cpp
    libs/hist/mpahistfile.cpp
    libs/hist/histmanager.cpp
    libs/hist/histinfo.cpp
    )
target_link_libraries(hist pixel)

add_library(pixel SHARED
    libs/pixel/abstractpixel.cpp
    libs/pixel/roipixel.cpp
    libs/pixel/cdbpixel.cpp
    )

#GUI
add_library(mainwindow SHARED
    #gui/mainwindow.cpp
    gui/mainwindow2.cpp
    )
target_link_libraries(mainwindow Qt5::Widgets hist plotwindow widgets tables)

add_library(plotwindow SHARED gui/plotwindow.cpp)
target_link_libraries(plotwindow hist qcustomplot)

add_library(tables SHARED
    gui/tables/histtable.cpp
    )
target_link_libraries(tables hist)

add_library(widgets SHARED
    gui/widgets/plotwidget.cpp
    gui/widgets/plotwidget2.cpp
    gui/widgets/mapplotwidget.cpp
    )
target_link_libraries(widgets hist qcustomplot)


# APPS
ADD_EXECUTABLE(cdbanalyzer main.cpp)
TARGET_LINK_LIBRARIES(cdbanalyzer mainwindow pixel )

INSTALL(TARGETS cdbanalyzer
    RUNTIME DESTINATION bin
)

#TESTS
add_executable(pixeltest test/pixeltest.cpp)
target_link_libraries(pixeltest pixel)

add_executable(maptest test/maptest.cpp)
target_link_libraries(maptest pixel)

add_executable(hist2dtest test/hist2dtest.cpp)
target_link_libraries(hist2dtest pixel hist)

add_executable(plottest test/plottest.cpp)
target_link_libraries(plottest plotwindow)

add_executable(fittest test/fittest.cpp)
target_link_libraries(fittest qcustomplot)

install(TARGETS pixeltest maptest RUNTIME DESTINATION bin)
