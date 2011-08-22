file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimerooptimizationalgs SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(calimerooptimizationalgs PROPERTIES PREFIX "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(calimerooptimizationalgs calimerocore)
else()
    target_link_libraries(calimerooptimizationalgs calimerocore ${Boost_THREAD_LIBRARY})
endif()

INSTALL(TARGETS calimerooptimizationalgs
        DESTINATION "share/calimero/libs"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
