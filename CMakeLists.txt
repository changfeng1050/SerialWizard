cmake_minimum_required(VERSION 3.9)
project(SerialWizard)

set(CMAKE_CXX_STANDARD 17)

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS_COVERAGE "${CMAKE_CXX_FLAGS_RELEASE} -fprofile-arcs -ftest-coverage")

set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(QT_DIR C:/Qt/Qt5.12.0/5.12.0/mingw73_32)
set(CMAKE_PREFIX_PATH ${QT_DIR} C:/Qt/Qt5.12.0/)

find_package(Qt5Widgets)
find_package(Qt5Core)
get_target_property(QtCore_location Qt5::Core LOCATION)
set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE")
link_directories(${QT_DIR}/lib C:/Qt/Qt5.12.0/Tools/mingw730_32/lib)
foreach (plugin ${Qt5Network_PLUGINS})
    get_target_property(_loc ${plugin} LOCATION)
    message("Plugin ${plugin} is at location ${_loc}")
endforeach ()


set(SOURCE_FILES
        main.cpp
        MainWindow.cpp
        MainWindow.h
        CalculateCheckSumDialog.cpp
        CalculateCheckSumDialog.h
        ConvertDataDialog.cpp
        ConvertDataDialog.h
        FrameInfoDialog.cpp
        FrameInfoDialog.h
        global.cpp
        global.h
        data/AbstractReadWriter.cpp
        data/AbstractReadWriter.h
        data/SerialReadWriter.cpp
        data/SerialReadWriter.h
        data/TcpServerReadWriter.cpp
        data/TcpServerReadWriter.h
        data/BridgeReadWriter.cpp
        data/BridgeReadWriter.h
        serial/SerialController.cpp
        serial/SerialController.h
        serial/NormalSerialController.cpp
        serial/NormalSerialController.h
        serial/LineSerialController.cpp
        serial/LineSerialController.h
        serial/FixedBytesSerialController.cpp
        serial/FixedBytesSerialController.h
        serial/FrameSerialController.cpp
        serial/FrameSerialController.h
        data/TcpClientReadWriter.cpp
        data/TcpClientReadWriter.h
        data/SerialBridgeReadWriter.cpp data/SerialBridgeReadWriter.h)

#add_executable(SerialWizard ${SOURCE_FILES})
add_executable(SerialWizard WIN32 ${SOURCE_FILES})
qt5_use_modules(SerialWizard Widgets Core SerialPort Network)