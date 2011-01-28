INCLUDE(${SWIG_USE_FILE})
EXECUTE_PROCESS(COMMAND ${SWIG_EXECUTABLE} -python -threads -external-runtime swigruntime.h)


ADD_CUSTOM_COMMAND(
        OUTPUT ${CMAKE_BINARY_DIR}/swigruntime.h
        COMMAND ${SWIG_EXECUTABLE} -python -threads -external-runtime swigruntime.h
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i
        )

INCLUDE_DIRECTORIES(${CMAKE_BINARY_DIR})

SET(CMAKE_SWIG_FLAGS "")
SET_SOURCE_FILES_PROPERTIES(pycalimero.i PROPERTIES CPLUSPLUS ON)
SET_SOURCE_FILES_PROPERTIES(pycalimero.i PROPERTIES SWIG_FLAGS "-threads")
SWIG_ADD_MODULE(pycalimero python pycalimero.i PyFunctionLoader.cpp PyEnv.cpp)
SWIG_LINK_LIBRARIES(pycalimero calimerocore ${PYTHON_LIBRARIES})
