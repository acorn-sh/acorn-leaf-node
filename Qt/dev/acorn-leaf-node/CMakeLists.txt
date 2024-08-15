cmake_minimum_required(VERSION 3.16)

project(acorn-leaf-node VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    mainbuttons.h mainbuttons.cpp
    containertable.h containertable.cpp
    terminal.h terminal.cpp
    settingspage.h settingspage.cpp
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(acorn-leaf-node
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        hubpage.h hubpage.cpp
        helppage.h helppage.cpp
    )
else()
    if(ANDROID)
        add_library(acorn-leaf-node SHARED ${PROJECT_SOURCES})
    else()
        add_executable(acorn-leaf-node ${PROJECT_SOURCES})
    endif()
endif()

target_link_libraries(acorn-leaf-node PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(acorn-leaf-node PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS acorn-leaf-node
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# Copy Python scripts and interpreter to the executable directory
# python3.11 -m venv bundled_python
# source bundled_python/bin/activate  # On Windows, use venv\Scripts\activate
# pip install pyinstaller
# pip install web3
# pip install "web3[tester]"

add_custom_command(TARGET acorn-leaf-node POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/bundled_python
        $<TARGET_FILE_DIR:acorn-leaf-node>/bundled_python
    COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/check_python.py
        ${CMAKE_SOURCE_DIR}/check_web3py.py
        ${CMAKE_SOURCE_DIR}/check_balance.py
        ${CMAKE_SOURCE_DIR}/check_docker.py
        ${CMAKE_SOURCE_DIR}/check_account.py
        ${CMAKE_SOURCE_DIR}/fetch_acornsh_docker_hub.py
        $<TARGET_FILE_DIR:acorn-leaf-node>
)


if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(acorn-leaf-node)
endif()
