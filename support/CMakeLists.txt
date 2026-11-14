# copy all from data to install dir /share
# here can be images, models

stage_biogears_schema(DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
stage_biogears_data(DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})


# The staging above only fills the build tree. Install the BioGears data next to the installed executable,
# which loads it relative to its working directory (e.g. ./states/StandardMale@0s.xml).
if(EXISTS "${Biogears_XML_DATA_DIR}")
  install(DIRECTORY "${Biogears_XML_DATA_DIR}/" DESTINATION bin FILES_MATCHING PATTERN "*.xml")
endif()
