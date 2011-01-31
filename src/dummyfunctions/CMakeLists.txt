file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(dummyfunctions SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(dummyfunctions PROPERTIES PREFIX "")

if (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
    target_link_libraries(dummyfunctions calimerocore)
else()
    target_link_libraries(dummyfunctions calimerocore ${Boost_THREAD_LIBRARY})
endif()

INSTALL(TARGETS dummyfunctions
        RUNTIME DESTINATION calimero
        LIBRARY DESTINATION calimero)
