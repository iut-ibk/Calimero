file(GLOB_RECURSE CPP_FILES ./ *.cpp)

add_library(calimerohelper SHARED ${CPP_FILES})

target_link_libraries(calimerohelper calimerocore)


INSTALL(TARGETS calimerohelper 
        DESTINATION bin
        PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE GROUP_READ GROUP_EXECUTE WORLD_READ WORLD_EXECUTE)
