#############################
# CMake Mod Manager root/src
#############################

set(PHYSIOLOGY_MANAGER_SOURCES PhysiologyManager.cpp AMM/PhysiologyEngineManager.cpp AMM/BiogearsThread.cpp)
set(PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
add_executable(${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES})
add_dependencies(${PHYSIOLOGY_MANAGER_EXE} stage_biogears_schema stage_biogears_data)
target_link_libraries(${PHYSIOLOGY_MANAGER_EXE}
        PUBLIC amm_std
        PUBLIC Threads::Threads
        PUBLIC Biogears::libbiogears
        PUBLIC Boost::system
        )

install(
   TARGETS ${PHYSIOLOGY_MANAGER_EXE}
   PERMISSIONS WORLD_READ WORLD_EXECUTE
   RUNTIME DESTINATION bin)
install(DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY} DESTINATION .)
