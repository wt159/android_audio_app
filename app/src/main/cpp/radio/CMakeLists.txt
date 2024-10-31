
ADD_SUBDIRECTORY(platform)

AUX_SOURCE_DIRECTORY(. MAIN_RADIO_SRC)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)

add_executable(si479x_radio ${MAIN_RADIO_SRC})
#add_executable(si479x_radio ./main.c)

#TARGET_LINK_LIBRARIES(si479x_radio ${PROJECT_BINARY_DIR}/bin/libsi479x.a)
#TARGET_LINK_LIBRARIES(si479x_radio ${PROJECT_BINARY_DIR}/bin/librds.a)
#TARGET_LINK_LIBRARIES(si479x_radio ${PROJECT_BINARY_DIR}/bin/libsi469x.a)
#TARGET_LINK_LIBRARIES(si479x_radio ${PROJECT_BINARY_DIR}/bin/libdcapp.a)
TARGET_LINK_LIBRARIES(si479x_radio platform dcapp si479x rds m)
#TARGET_LINK_LIBRARIES(si479x_radio platform dcapp si479x rds si469x m)
