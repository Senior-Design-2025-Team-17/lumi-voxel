# THIS FILE IS AUTOMATICALLY GENERATED. DO NOT EDIT.
# BASED ON c:\Users\Aidan\OneDrive\Purdue\Spring 2025\ECE 477\lumi-voxel

function(add_st_target_properties TARGET_NAME)

target_compile_definitions(
    ${TARGET_NAME} PRIVATE
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:ASM>>:DEBUG>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:ASM>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:DEBUG>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:USE_HAL_DRIVER>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:STM32H725xx>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:USE_PWR_DIRECT_SMPS_SUPPLY>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:DEBUG>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:USE_HAL_DRIVER>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:STM32H725xx>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:USE_PWR_DIRECT_SMPS_SUPPLY>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:ASM>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:USE_HAL_DRIVER>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:STM32H725xx>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:USE_PWR_DIRECT_SMPS_SUPPLY>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:USE_HAL_DRIVER>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:STM32H725xx>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:STM32_PROCESSOR=h7>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:USE_PWR_DIRECT_SMPS_SUPPLY>"
)

target_include_directories(
    ${TARGET_NAME} PRIVATE
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:ASM>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Core\\Inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc\\Legacy>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Include>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\Target>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\utils>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\includes>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\hci\\hci_tl_patterns\\Basic>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\App>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\BSP\\custom>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Core\\Inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc\\Legacy>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Include>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\Target>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\utils>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\includes>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\hci\\hci_tl_patterns\\Basic>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\App>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\BSP\\custom>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:ASM>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Core\\Inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc\\Legacy>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Include>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\Target>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\utils>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\includes>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\hci\\hci_tl_patterns\\Basic>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\App>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:${PROJECT_SOURCE_DIR}/Drivers\\BSP\\custom>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Core\\Inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\STM32H7xx_HAL_Driver\\Inc\\Legacy>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Device\\ST\\STM32H7xx\\Include>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\CMSIS\\Include>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/lib/common-lib/inc>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\Target>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\utils>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\includes>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Middlewares\\ST\\BlueNRG-2\\hci\\hci_tl_patterns\\Basic>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/BlueNRG_2\\App>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:${PROJECT_SOURCE_DIR}/Drivers\\BSP\\custom>"
)

target_compile_options(
    ${TARGET_NAME} PRIVATE
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:ASM>>:-g3>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-g3>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-g3>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:ASM>>:-g0>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:-g0>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:-g0>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:-O0>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:-O0>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:-Os>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:-Os>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:C>>:>"
    "$<$<AND:$<CONFIG:Debug>,$<COMPILE_LANGUAGE:CXX>>:>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:C>>:>"
    "$<$<AND:$<NOT:$<CONFIG:Debug>>,$<COMPILE_LANGUAGE:CXX>>:>"
    "$<$<CONFIG:Debug>:-mcpu=cortex-m7>"
    "$<$<CONFIG:Debug>:-mfpu=fpv5-d16>"
    "$<$<CONFIG:Debug>:-mfloat-abi=hard>"
    "$<$<NOT:$<CONFIG:Debug>>:-mcpu=cortex-m7>"
    "$<$<NOT:$<CONFIG:Debug>>:-mfpu=fpv5-d16>"
    "$<$<NOT:$<CONFIG:Debug>>:-mfloat-abi=hard>"
)

target_link_libraries(
    ${TARGET_NAME} PRIVATE
)

target_link_directories(
    ${TARGET_NAME} PRIVATE
)

target_link_options(
    ${TARGET_NAME} PRIVATE
    "$<$<CONFIG:Debug>:-mcpu=cortex-m7>"
    "$<$<CONFIG:Debug>:-mfpu=fpv5-d16>"
    "$<$<CONFIG:Debug>:-mfloat-abi=hard>"
    "$<$<NOT:$<CONFIG:Debug>>:-mcpu=cortex-m7>"
    "$<$<NOT:$<CONFIG:Debug>>:-mfpu=fpv5-d16>"
    "$<$<NOT:$<CONFIG:Debug>>:-mfloat-abi=hard>"
    -T
    "$<$<CONFIG:Debug>:${PROJECT_SOURCE_DIR}/STM32H725RGVX_FLASH.ld>"
    "$<$<NOT:$<CONFIG:Debug>>:${PROJECT_SOURCE_DIR}/STM32H725RGVX_FLASH.ld>"
)

target_sources(
    ${TARGET_NAME} PRIVATE
    "lib\\common-lib\\src\\errors.cpp"
    "lib\\common-lib\\src\\high_precision_counter.cpp"
    "lib\\common-lib\\src\\interrupt_queue.cpp"
    "lib\\common-lib\\src\\scheduler.cpp"
    "lib\\common-lib\\src\\syscall_retarget.cpp"
    "lib\\common-lib\\src\\timer_helpers.c"
    "BlueNRG_2\\App\\app_bluenrg_2.c"
    "BlueNRG_2\\App\\gatt_db.c"
    "BlueNRG_2\\Target\\hci_tl_interface.c"
    "Core\\Src\\custom_bus.c"
    "Core\\Src\\lp5899.cpp"
    "Core\\Src\\main.c"
    "Core\\Src\\run.cpp"
    "Core\\Src\\stm32h7xx_hal_msp.c"
    "Core\\Src\\stm32h7xx_it.c"
    "Core\\Src\\syscalls.c"
    "Core\\Src\\sysmem.c"
    "Core\\Src\\system_stm32h7xx.c"
    "Core\\Startup\\startup_stm32h725rgvx.s"
    "Middlewares\\ST\\BlueNRG-2\\hci\\bluenrg1_devConfig.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\bluenrg1_events_cb.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\bluenrg1_events.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\bluenrg1_hci_le.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\controller\\bluenrg1_gap_aci.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\controller\\bluenrg1_gatt_aci.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\controller\\bluenrg1_hal_aci.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\controller\\bluenrg1_l2cap_aci.c"
    "Middlewares\\ST\\BlueNRG-2\\hci\\hci_tl_patterns\\Basic\\hci_tl.c"
    "Middlewares\\ST\\BlueNRG-2\\utils\\ble_list.c"
    "Drivers\\BSP\\custom\\custom.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_adc_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_adc.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_cordic.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_cortex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_crc_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_crc.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_dma_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_dma.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_exti.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_flash_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_flash.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_gpio.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_hsem.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_i2c_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_i2c.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_mdma.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_pwr_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_pwr.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_rcc_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_rcc.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_spi_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_spi.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_tim_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_tim.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_uart_ex.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal_uart.c"
    "Drivers\\STM32H7xx_HAL_Driver\\Src\\stm32h7xx_hal.c"
)

add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_SIZE} $<TARGET_FILE:${TARGET_NAME}>
)

add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O ihex
    $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.hex
)

add_custom_command(
    TARGET ${TARGET_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
    $<TARGET_FILE:${TARGET_NAME}> ${TARGET_NAME}.bin
)

endfunction()