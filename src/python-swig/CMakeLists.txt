include(${SWIG_USE_FILE})
execute_process(COMMAND ${SWIG_EXECUTABLE} -python -external-runtime swigruntime.h)

#execute_process(COMMAND ${SWIG_EXECUTABLE} -python -c++ -o pycalimeroWRAP.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i
#	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_BINARY_DIR}/pycalimeroWRAP.cpp
	COMMAND ${SWIG_EXECUTABLE} -python -c++ -o ${CMAKE_BINARY_DIR}/pycalimeroWRAP.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i
	)

include_directories(${CMAKE_BINARY_DIR})
ADD_CUSTOM_TARGET(pycalimerogen DEPENDS ${CMAKE_BINARY_DIR}/pycalimeroWRAP.cpp)


#add_custom_target(pycalimerogen ALL ${SWIG_EXECUTABLE} -python -c++ -o pycalimeroWRAP.cpp ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i
#	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
#	SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/pycalimero.i)

SET_SOURCE_FILES_PROPERTIES(pycalimero.i PROPERTIES CPLUSPLUS ON)
SWIG_ADD_MODULE(pycalimero python pycalimero.i PyFunctionLoader.cpp PyEnv.cpp)
SWIG_LINK_LIBRARIES(pycalimero calimerocore ${PYTHON_LIBRARIES})
