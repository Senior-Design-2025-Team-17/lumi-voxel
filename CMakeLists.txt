cmake_minimum_required(VERSION 3.20)

project("lumi-voxel" C CXX ASM)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/st-project.cmake)

add_executable(${PROJECT_NAME})
add_st_target_properties(${PROJECT_NAME})

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_STRIP} -S -o ${PROJECT_NAME}.stripped $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_OBJDUMP} -CxD ${PROJECT_NAME}.stripped > ${PROJECT_NAME}.list

    COMMAND ${CMAKE_STRIP} -S -R .ARM.attributes -R .comment -o ${PROJECT_NAME}.stripped $<TARGET_FILE:${PROJECT_NAME}>
    COMMAND ${CMAKE_SIZE} -A ${PROJECT_NAME}.stripped
    COMMAND ${CMAKE_SIZE} ${PROJECT_NAME}.stripped
    COMMAND arm-none-eabi-readelf -l ${PROJECT_NAME}.elf > ${PROJECT_NAME}.readelf
)