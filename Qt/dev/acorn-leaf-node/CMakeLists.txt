cmake_minimum_required(VERSION 3.16)

project(acorn-leaf-node VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find and include the required Qt components, including QtConcurrent
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)

set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    MainButtons.h MainButtons.cpp
    DashboardTable.h DashboardTable.cpp
    Terminal.h Terminal.cpp
    SettingsPage.h SettingsPage.cpp
    resources.qrc
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(acorn-leaf-node
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        HubPage.h HubPage.cpp
        HelpPage.h HelpPage.cpp
        AccountPage.h AccountPage.cpp
        ResourceManager.h ResourceManager.cpp
        BlockchainManager.h BlockchainManager.cpp
        PaymentManager.h PaymentManager.cpp
        PythonServiceManager.h PythonServiceManager.cpp
        ContainerInfo.h
        DashboardPage.h DashboardPage.cpp
        HubTable.h HubTable.cpp
        Components.h Components.cpp
    )
else()
    if(ANDROID)
        add_library(acorn-leaf-node SHARED ${PROJECT_SOURCES})
    else()
        add_executable(acorn-leaf-node ${PROJECT_SOURCES})
    endif()
endif()

# Link against the required Qt libraries, including QtConcurrent
target_link_libraries(acorn-leaf-node PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent)

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
        ${CMAKE_SOURCE_DIR}/generate_work_address.py
        ${CMAKE_SOURCE_DIR}/docker_commands.py
        ${CMAKE_SOURCE_DIR}/docker_containers.py
        ${CMAKE_SOURCE_DIR}/docker_images.py
        ${CMAKE_SOURCE_DIR}/eth_account.json
        $<TARGET_FILE_DIR:acorn-leaf-node>
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(acorn-leaf-node)
endif()
