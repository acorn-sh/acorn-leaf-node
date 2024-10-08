cmake_minimum_required(VERSION 3.16)

project(acorn-leaf-node VERSION 0.1 LANGUAGES CXX)

# Enable automatic handling of Qt features like UIC, MOC, and RCC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Qt package
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Concurrent)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Concurrent)

# List of project source files
set(PROJECT_SOURCES
    main.cpp
    MainWindow.cpp MainWindow.h MainWindow.ui
    MainButtons.h MainButtons.cpp
    DashboardTable.h DashboardTable.cpp
    Terminal.h Terminal.cpp
    SettingsPage.h SettingsPage.cpp
    resources.qrc
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
    TrayIcon.h TrayIcon.cpp
)

# Define the target executable
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(acorn-leaf-node
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        TimeTracker.h TimeTracker.cpp
        HashProof.h HashProof.cpp
        Wallet.h Wallet.cpp
        CardButton.h CardButton.cpp


    )
else()
    if(ANDROID)
        add_library(acorn-leaf-node SHARED ${PROJECT_SOURCES})
    else()
        add_executable(acorn-leaf-node ${PROJECT_SOURCES})
    endif()
endif()

# Link libraries
target_link_libraries(acorn-leaf-node PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Concurrent)

# Platform-specific properties
set_target_properties(acorn-leaf-node PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

# Installation paths
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

# Set the RPATH for macOS to look for libraries within the app bundle
if(APPLE)
    set_target_properties(acorn-leaf-node PROPERTIES
        INSTALL_RPATH "@executable_path/../bundled_python/lib"
        BUILD_WITH_INSTALL_RPATH TRUE
        MACOSX_RPATH TRUE
    )
endif()

# Set the PATH for Windows to include the bundled Python directories
if(WIN32)
    # Set the PATH to include the directories where the Python DLLs are located
    set_target_properties(acorn-leaf-node PROPERTIES
        VS_DEBUGGER_ENVIRONMENT "PATH=%PATH%;%EXEPATH%\\bundled_python\\DLLs;%EXEPATH%\\bundled_python\\Scripts;%EXEPATH%\\bundled_python\\Lib"
    )
endif()

# Copy Python shared libraries for macOS
if(APPLE)
    add_custom_command(TARGET acorn-leaf-node POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/bundled_python/lib
            $<TARGET_FILE_DIR:acorn-leaf-node>/../bundled_python/lib
    )
endif()

# Copy Python shared libraries for Windows
if(WIN32)
    add_custom_command(TARGET acorn-leaf-node POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/bundled_python/DLLs
            $<TARGET_FILE_DIR:acorn-leaf-node>/bundled_python/DLLs
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/bundled_python/Lib
            $<TARGET_FILE_DIR:acorn-leaf-node>/bundled_python/Lib
        COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_SOURCE_DIR}/bundled_python/Scripts
            $<TARGET_FILE_DIR:acorn-leaf-node>/bundled_python/Scripts
    )
endif()

# Platform-specific icon handling
if(APPLE)
    set(MACOSX_BUNDLE_ICON_FILE "AppIcon.icns")
    set_source_files_properties(${CMAKE_SOURCE_DIR}/output_icons/macos/AppIcon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
    target_sources(acorn-leaf-node PRIVATE ${CMAKE_SOURCE_DIR}/output_icons/macos/AppIcon.icns)
endif()

if(WIN32)
    set(WINDOWS_ICON ${CMAKE_SOURCE_DIR}/output_icons/windows/AppIcon.ico)
    set_target_properties(acorn-leaf-node PROPERTIES WIN32_EXECUTABLE TRUE)
    target_sources(acorn-leaf-node PRIVATE ${WINDOWS_ICON})
endif()

# Linux-specific icon installation
if(UNIX AND NOT APPLE)
    foreach(ICON_SIZE 16 32 48 64 128 256)
        install(FILES ${CMAKE_SOURCE_DIR}/output_icons/linux/${ICON_SIZE}x${ICON_SIZE}.png
                DESTINATION /usr/share/icons/hicolor/${ICON_SIZE}x${ICON_SIZE}/apps
                RENAME acorn-leaf-node.png)
    endforeach()
endif()

# Finalize executable for Qt6
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(acorn-leaf-node)
endif()
