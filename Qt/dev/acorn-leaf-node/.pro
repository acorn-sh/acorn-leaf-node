QT += core gui

CONFIG += c++11

# Define the specific Python version path
PYTHON_PATH = /opt/homebrew/bin/python3.11

# Use the Python path
QMAKE_CXXFLAGS += -I$$system($$PYTHON_PATH -c "from distutils.sysconfig import get_python_inc; print(get_python_inc())")
QMAKE_LFLAGS += -L$$system($$PYTHON_PATH -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")

SOURCES += main.cpp
