CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
PROJECT(calimero)

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        MESSAGE(STATUS "Using Visual C compilers")
        SET(CMAKE_CXX_FLAGS "/EHsc")
ENDIF()

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
        MESSAGE(STATUS "Using GNU compilers")
        SET(CMAKE_CXX_FLAGS "-g -Wall -O3")
ENDIF()

INCLUDE_DIRECTORIES(src/calimerocore
                    src/python-swig
                    src/plugins
                    src/plugins/nativefunctions
                    src/plugins/optimizationalgs
                    src/plugins/calimerohelper
                    src/plugins/dummyfunctions
                    src/calimerohelper
                    )

FIND_PACKAGE(Qt4 REQUIRED)

SET(Boost_USE_STATIC_LIBS ON)
SET(Boost_USE_MULTITHREADED ON)
SET(Boost_USE_STATIC_RUNTIME OFF)
FIND_PACKAGE(Boost 1.38.0 COMPONENTS chrono system signals program_options thread date_time filesystem REQUIRED)

ADD_DEFINITIONS(${Boost_LIB_DIAGNOSTIC_DEFINITIONS})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})

FIND_PACKAGE(SWIG REQUIRED)

FIND_PACKAGE(PythonLibs 2.7 REQUIRED)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

if (CMAKE_BUILD_TYPE STREQUAL Debug)
        MESSAGE(STATUS Debug)
        ADD_DEFINITIONS(-DDEBUG)
else ()
        MESSAGE(STATUS Release)
endif()

OPTION(WITH_PARALLEL "Enable parallel code. Needs a OpenMP compiler" ON)
FIND_PACKAGE(OpenMP QUIET)
IF(${WITH_PARALLEL} AND ${OPENMP_FOUND} AND NOT (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC))
        SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
        ADD_DEFINITIONS(-DOPENMP_ENABLED)
        MESSAGE(STATUS "OpenMP is enabled")
ELSE()
        ADD_DEFINITIONS(-DOPENMP_DISABLED)
        MESSAGE(STATUS "OpenMP is disabled. Consider using a compiler that supports OpenMP")
ENDIF()

MESSAGE(STATUS ${CMAKE_CXX_COMPILER_ID})
SET(QT_USE_QTXML true)
SET(QT_USE_QTSVG true)
SET(QT_USE_QTSQL true)
SET(QT_NOTUSE_QTGUI true)
ADD_DEFINITIONS(-DQT_NO_KEYWORDS)
INCLUDE(${QT_USE_FILE})
SET(LIBRARY_OUTPUT_PATH ${calimero_BINARY_DIR})
SET(EXECUTABLE_OUTPUT_PATH ${calimero_BINARY_DIR})

ADD_SUBDIRECTORY(src/calimerocore)
ADD_SUBDIRECTORY(src/python-swig)
ADD_SUBDIRECTORY(src/plugins)
ADD_SUBDIRECTORY(src/app)
ADD_SUBDIRECTORY(src/calimerohelper)
ADD_SUBDIRECTORY(scripts)
ADD_SUBDIRECTORY(site-packages)
INCLUDE(CalimeroCPack.txt)
