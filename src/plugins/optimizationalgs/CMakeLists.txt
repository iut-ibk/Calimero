file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimerooptimizationalgs SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(calimerooptimizationalgs PROPERTIES PREFIX "")

target_link_libraries(calimerooptimizationalgs calimerocore)

INSTALL(TARGETS calimerooptimizationalgs
        DESTINATION "share/calimero/libs"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
