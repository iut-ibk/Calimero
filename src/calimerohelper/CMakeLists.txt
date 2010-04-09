file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimerohelper SHARED ${CPP_FILES})

IF (MINGW)
	SET_TARGET_PROPERTIES(calimerohelper PROPERTIES PREFIX "")
endif()
if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(calimerohelper calimerocore)
else()
    target_link_libraries(calimerohelper calimerocore ${Boost_THREAD_LIBRARY})
endif()


INSTALL(TARGETS calimerohelper 
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib)
