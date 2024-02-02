
# ---------------------------------------- subdirectories --------------------------------------------------------------
# ======================================================================================================================

add_subdirectory(cxxopts EXCLUDE_FROM_ALL)


# ---------------------------------------- link libraries --------------------------------------------------------------
# ======================================================================================================================

add_subdirectory(cxxsemaphore)
add_subdirectory(cxxshm)

target_link_libraries(${Target} PRIVATE rt)
target_link_libraries(${Target} PRIVATE cxxopts)
target_link_libraries(${Target} PRIVATE cxxsemaphore)
target_link_libraries(${Target} PRIVATE cxxshm)
