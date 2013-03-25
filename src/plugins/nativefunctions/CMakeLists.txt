file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimeronativefunctions SHARED ${CPP_FILES})

SET_TARGET_PROPERTIES(calimeronativefunctions PROPERTIES PREFIX "")

target_link_libraries(calimeronativefunctions calimerocore)


INSTALL(TARGETS calimeronativefunctions
        DESTINATION "share/calimero/libs"
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
