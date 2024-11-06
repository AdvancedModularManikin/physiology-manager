#############################
# CMake Mod Manager root/src
#############################

set(PHYSIOLOGY_MANAGER_SOURCES PhysiologyManager.cpp AMM/PhysiologyEngineManager.cpp AMM/BiogearsThread.cpp
        AMM/MoHSES_EventHandler.cpp)
set(PHYSIOLOGY_MANAGER_EXE amm_physiology_manager)
add_executable(${PHYSIOLOGY_MANAGER_EXE} ${PHYSIOLOGY_MANAGER_SOURCES})
add_dependencies(${PHYSIOLOGY_MANAGER_EXE} stage_biogears_schema stage_biogears_data)
target_link_libraries(${PHYSIOLOGY_MANAGER_EXE}
        PUBLIC amm_std
        PUBLIC Threads::Threads
        PUBLIC Biogears::libbiogears
        PUBLIC Boost::system
	PUBLIC tinyxml2
        )

install(
   TARGETS ${PHYSIOLOGY_MANAGER_EXE}
   RUNTIME DESTINATION bin)
install(DIRECTORY ../config DESTINATION bin)
