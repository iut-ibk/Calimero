file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(dummyfunctions SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(dummyfunctions PROPERTIES PREFIX "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(dummyfunctions calimerocore)
else()
    target_link_libraries(dummyfunctions calimerocore ${Boost_THREAD_LIBRARY})
endif()

INSTALL(TARGETS dummyfunctions
        DESTINATION "share/calimero/libs"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
