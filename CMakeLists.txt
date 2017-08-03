cmake_minimum_required(VERSION 3.6)
project(SerialWizard)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_DIR C:/Qt/Qt5.7.0/5.7/mingw530_32)
set(CMAKE_PREFIX_PATH ${QT_DIR} C:/Qt/Qt5.7.0/)

find_package(Qt5Widgets)
find_package(Qt5Core)
find_package(QtSerialPort)
get_target_property(QtCore_location Qt5::Core LOCATION)
set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE")
link_directories(${QT_DIR}/lib C:/Qt/Qt5.7.0/Tools/mingw530_32/lib)
foreach (plugin ${Qt5Network_PLUGINS})
    get_target_property(_loc ${plugin} LOCATION)
    message("Plugin ${plugin} is at location ${_loc}")
endforeach ()


set(SOURCE_FILES main.cpp SerialPort.cpp SerialPort.h MainWindow.cpp MainWindow.h CalculateCheckSumDialog.cpp CalculateCheckSumDialog.h FrameInfoDialog.cpp FrameInfoDialog.h global.cpp global.h)
add_executable(SerialWizard ${SOURCE_FILES})
qt5_use_modules(SerialWizard Widgets Core SerialPort)