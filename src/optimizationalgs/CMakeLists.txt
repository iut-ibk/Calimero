file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimerooptimizationalgs SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(calimerooptimizationalgs PROPERTIES PREFIX "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(calimerooptimizationalgs calimerocore)
else()
    target_link_libraries(calimerooptimizationalgs calimerocore ${Boost_THREAD_LIBRARY})
endif()

INSTALL(TARGETS calimerooptimizationalgs
        RUNTIME DESTINATION calimero
        LIBRARY DESTINATION calimero)
