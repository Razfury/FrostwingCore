if(NOT BUILD_REALMSCRIPTS )
  message("\tRealmScripts was skipped because BUILD_REALMSCRIPTS is unchecked.")
  return()
endif(NOT BUILD_REALMSCRIPTS)

PROJECT(RealmScripts)
SET(prefix ${SCRIPTS_PATH}/RealmScripts)

file(GLOB_RECURSE SRC_SETUP
    "${SCRIPTS_PATH}/RealmScripts/Setup/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/Setup/*.h"
)
file(GLOB_RECURSE SRC_OUTLAND
    "${SCRIPTS_PATH}/RealmScripts/Outland/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/Outland/*.h"
)
file(GLOB_RECURSE SRC_NORTHREND
    "${SCRIPTS_PATH}/RealmScripts/Northrend/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/Northrend/*.h"
)
file(GLOB_RECURSE SRC_KALIMDOR
    "${SCRIPTS_PATH}/RealmScripts/Kalimdor/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/Kalimdor/*.h"
)
file(GLOB_RECURSE SRC_EASTERNKINGDOMS
    "${SCRIPTS_PATH}/RealmScripts/EasternKingdoms/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/EasternKingdoms/*.h"
)
file(GLOB_RECURSE SRC_MISC
    "${SCRIPTS_PATH}/RealmScripts/Misc/*.cpp"
    "${SCRIPTS_PATH}/RealmScripts/Misc/*.h"
)

include_directories(${include_dirs} ${prefix} ${GLOBAL_INCLUDE_DIRS})
add_library(${PROJECT_NAME} SHARED ${SRC_SETUP} ${SRC_OUTLAND} ${SRC_NORTHREND} ${SRC_KALIMDOR} ${SRC_EASTERNKINGDOMS} ${SRC_MISC})
add_dependencies(${PROJECT_NAME} ${dependancies})
target_link_libraries(${PROJECT_NAME} ${linkages})

source_group("Setup"            FILES ${SRC_SETUP})
source_group("Outland"            FILES ${SRC_OUTLAND})
source_group("Northrend"        FILES ${SRC_NORTHREND})
source_group("Kalimdor"        FILES ${SRC_KALIMDOR})
source_group("EasternKingdoms"        FILES ${SRC_EASTERNKINGDOMS})
source_group("Misc"        FILES ${SRC_MISC})

include(PCHSupport)
#IF(CMAKE_GENERATOR MATCHES Visual*)
	#ADD_NATIVE_PRECOMPILED_HEADER(${PROJECT_NAME} Setup.h ${prefix}/Setup.cpp)
#ENDIF()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION ${FROSTWINGCORE_SCRIPTLIB_PATH} LIBRARY DESTINATION ${FROSTWINGCORE_SCRIPTLIB_PATH})
#Cleap up our variables(Not required.)
unset(prefix)
unset(SRCS)
unset(sources)