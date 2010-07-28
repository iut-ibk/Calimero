FILE(GLOB_RECURSE CPP_FILES ./ *.cpp)
ADD_LIBRARY(pycalimero SHARED ${CPP_FILES})
TARGET_LINK_LIBRARIES(pycalimero ${PYTHON_LIBRARIES} calimerocore ${Boost_PYTHON_LIBRARY})

if(WIN32)
    GET_TARGET_PROPERTY(LIB_NAME pycalimero LOCATION)
    SET(NEW_LIB_NAME  ${CMAKE_BINARY_DIR}/pycalimero.pyd)
    ADD_CUSTOM_COMMAND(
       TARGET pycalimero
       POST_BUILD
       COMMAND ${CMAKE_COMMAND} -E copy ${LIB_NAME} ${NEW_LIB_NAME})
else()
        SET_TARGET_PROPERTIES(pycalimero
        PROPERTIES
        OUTPUT_NAME "pycalimero"
        PREFIX ""
)
endif()

INSTALL(TARGETS pycalimero
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION calimero)
