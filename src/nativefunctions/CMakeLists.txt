file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimeronativefunctions SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(calimeronativefunctions PROPERTIES PREFIX "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${OpenMP_CXX_FLAGS}")
    target_link_libraries(calimeronativefunctions calimerocore)
else()
    target_link_libraries(calimeronativefunctions calimerocore ${Boost_THREAD_LIBRARY})
endif()

INSTALL(TARGETS calimeronativefunctions
        RUNTIME DESTINATION calimero
        LIBRARY DESTINATION calimero)
